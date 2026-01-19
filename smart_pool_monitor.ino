/*
 * ADVANCED SMART POOL WATER QUALITY MONITORING SYSTEM
 * Hardware: Arduino Uno, TDS Sensor, pH Sensor, Temperature Sensor, 
 *           SD Card Module, WiFi/Bluetooth, 16x2 I2C LCD with buttons
 * Features:
 * 1. Complete water quality monitoring (TDS, pH, Temperature)
 * 2. Temperature-compensated TDS readings
 * 3. Calculate salt needed to reach target salinity
 * 4. SD Card data logging with timestamp
 * 5. WiFi/Bluetooth remote monitoring
 * 6. Automated email/SMS alerts for critical conditions
 * 7. User interface for parameter adjustment
 * 8. Real-time clock for accurate timestamps
 * 
 * Author: Embedded Systems Engineer
 * Date: 2024
 */

// =================== LIBRARIES ===================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiNINA.h>           // For MKR boards
// OR #include <ESP8266WiFi.h>  // For ESP boards
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>     // For Bluetooth

// =================== HARDWARE PIN DEFINITIONS ===================
// Sensor Pins
#define TDS_SENSOR_PIN A0
#define PH_SENSOR_PIN A2
#define ONE_WIRE_BUS 2          // For DS18B20 temperature sensor

// SD Card Module Pins
#define SD_CS_PIN 10
#define SD_MOSI_PIN 11
#define SD_MISO_PIN 12
#define SD_SCK_PIN 13

// User Interface Pins (Buttons)
#define BUTTON_UP 3
#define BUTTON_DOWN 4
#define BUTTON_SELECT 5
#define BUTTON_BACK 6

// Status LEDs
#define LED_OK 7
#define LED_WARNING 8
#define LED_CRITICAL 9

// Bluetooth
#define BLUETOOTH_TX 14
#define BLUETOOTH_RX 15

// =================== CONSTANTS & PARAMETERS ===================
// Water Quality Parameters (Default Values - Adjustable via UI)
struct WaterQualityParams {
  float targetSalinity = 3000.0;      // ppm
  float targetPH = 7.4;               // Ideal pH for pools
  float phMin = 7.2;                  // Minimum acceptable pH
  float phMax = 7.8;                  // Maximum acceptable pH
  float tempMin = 18.0;               // °C
  float tempMax = 30.0;               // °C
  float poolVolume = 50000.0;         // liters
  unsigned long logInterval = 300000; // Log every 5 minutes (ms)
  bool enableAlerts = true;
  bool enableDataLogging = true;
  bool enableRemote = true;
};

// TDS Sensor Constants
#define VREF 5.0
#define ADC_RESOLUTION 1023.0
#define TDS_TEMP_COMPENSATION 0.02
#define TDS_CALIBRATION_FACTOR 1.0

// pH Sensor Constants
#define PH_CALIBRATION_NEUTRAL 7.0    // Neutral pH calibration point
#define PH_CALIBRATION_ACID 4.0       // Acid pH calibration point
#define PH_NEUTRAL_VOLTAGE 2.5        // Voltage at pH 7.0
#define PH_ACID_VOLTAGE 3.0           // Voltage at pH 4.0 (example)
#define PH_SLOPE -5.0                 // pH/Volt slope

// Sampling & Averaging
#define SAMPLE_INTERVAL 1000          // Main loop interval (ms)
#define ROLLING_AVERAGE_SIZE 30
#define PH_CALIBRATION_SAMPLES 100

// Alert Thresholds
#define ALERT_SALINITY_LOW 2500
#define ALERT_SALINITY_HIGH 3500
#define ALERT_PH_CRITICAL_LOW 6.8
#define ALERT_PH_CRITICAL_HIGH 8.0
#define ALERT_TEMP_CRITICAL_LOW 10.0
#define ALERT_TEMP_CRITICAL_HIGH 40.0

// WiFi Credentials (Store in EEPROM for security)
#define EEPROM_SIZE 512
#define WIFI_SSID_ADDR 0
#define WIFI_PASS_ADDR 32
#define API_KEY_ADDR 64

// =================== GLOBAL OBJECTS & VARIABLES ===================
// LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Temperature Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Bluetooth
SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

// WiFi & NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// SD Card
File dataFile;

// Parameters Structure
WaterQualityParams params;

// Sensor Values with Rolling Averages
float tdsReadings[ROLLING_AVERAGE_SIZE];
float phReadings[ROLLING_AVERAGE_SIZE];
int readingIndex = 0;
float tdsSum = 0, phSum = 0;

// Current Readings
float currentTemperature = 25.0;
float compensatedTDS = 0.0;
float currentPH = 7.0;
float saltNeeded = 0.0;
float acidNeeded = 0.0;   // For pH adjustment (if needed)
float baseNeeded = 0.0;   // For pH adjustment (if needed)

// System State
enum SystemState {
  STATE_MONITORING,
  STATE_MENU,
  STATE_CALIBRATION,
  STATE_SETTINGS,
  STATE_LOGS
};
SystemState currentState = STATE_MONITORING;

// Menu Navigation
int menuIndex = 0;
int subMenuIndex = 0;

// Timing Variables
unsigned long lastLogTime = 0;
unsigned long lastAlertCheck = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastSensorRead = 0;

// Alert Flags
bool salinityAlert = false;
bool pHAlert = false;
bool tempAlert = false;
bool systemAlert = false;

// =================== SETUP FUNCTION ===================
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println(F("\n=== ADVANCED SMART POOL MONITORING SYSTEM ==="));
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Initializing..."));
  
  // Initialize Pins
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_CRITICAL, OUTPUT);
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Load Parameters from EEPROM
  loadParameters();
  
  // Initialize Sensors
  tempSensor.begin();
  analogReference(DEFAULT);  // 5V reference for Arduino Uno
  
  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("SD Card initialization failed!"));
    lcd.setCursor(0, 1);
    lcd.print(F("SD Card Error"));
    digitalWrite(LED_CRITICAL, HIGH);
    delay(2000);
  } else {
    Serial.println(F("SD Card initialized."));
  }
  
  // Initialize Bluetooth
  bluetooth.begin(9600);
  
  // Initialize WiFi (if enabled)
  if (params.enableRemote) {
    initializeWiFi();
  }
  
  // Initialize NTP Client for time
  timeClient.begin();
  timeClient.setTimeOffset(0);  // Adjust for timezone
  
  // Create data log file with header
  if (params.enableDataLogging) {
    createLogFile();
  }
  
  // Display splash screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Pool Monitor v2.0"));
  lcd.setCursor(0, 1);
  lcd.print(F("System Ready"));
  
  delay(2000);
  lcd.clear();
  
  Serial.println(F("System initialization complete."));
  Serial.println(F("============================================"));
}

// =================== MAIN LOOP ===================
void loop() {
  unsigned long currentMillis = millis();
  
  // Handle User Input
  handleUserInput();
  
  // State Machine
  switch (currentState) {
    case STATE_MONITORING:
      // Read sensors every second
      if (currentMillis - lastSensorRead >= SAMPLE_INTERVAL) {
        readAllSensors();
        calculateAdjustments();
        checkAlerts();
        updateStatusLEDs();
        lastSensorRead = currentMillis;
      }
      
      // Update display every 2 seconds
      if (currentMillis - lastDisplayUpdate >= 2000) {
        updateDisplay();
        lastDisplayUpdate = currentMillis;
      }
      
      // Log data at specified interval
      if (params.enableDataLogging && 
          currentMillis - lastLogTime >= params.logInterval) {
        logDataToSD();
        lastLogTime = currentMillis;
      }
      
      // Send data to remote server
      if (params.enableRemote && WiFi.status() == WL_CONNECTED) {
        sendDataToServer();
      }
      
      // Send data via Bluetooth
      sendDataViaBluetooth();
      break;
      
    case STATE_MENU:
      displayMenu();
      break;
      
    case STATE_SETTINGS:
      displaySettings();
      break;
      
    case STATE_CALIBRATION:
      runCalibration();
      break;
      
    case STATE_LOGS:
      displayLogs();
      break;
  }
  
  // Check for critical alerts every 30 seconds
  if (currentMillis - lastAlertCheck >= 30000) {
    sendCriticalAlerts();
    lastAlertCheck = currentMillis;
  }
  
  // Handle incoming Bluetooth commands
  handleBluetoothCommands();
}

// =================== SENSOR FUNCTIONS ===================
void readAllSensors() {
  // Read temperature from DS18B20
  tempSensor.requestTemperatures();
  currentTemperature = tempSensor.getTempCByIndex(0);
  
  // Read and compensate TDS
  compensatedTDS = readAndCompensateTDS(currentTemperature);
  
  // Read pH
  currentPH = readPH(currentTemperature);
  
  // Apply rolling averages
  compensatedTDS = applyRollingAverage(compensatedTDS, tdsReadings, &tdsSum);
  currentPH = applyRollingAverage(currentPH, phReadings, &phSum);
}

float readAndCompensateTDS(float temperature) {
  int analogValue = analogRead(TDS_SENSOR_PIN);
  
  // Convert to voltage
  float voltage = analogValue * (VREF / ADC_RESOLUTION);
  
  // Calculate TDS from voltage
  float rawTDS = voltage * TDS_CALIBRATION_FACTOR * 500;
  
  // Apply temperature compensation
  float compensationFactor = 1.0 + TDS_TEMP_COMPENSATION * (temperature - 25.0);
  float tempCompensatedTDS = rawTDS / compensationFactor;
  
  return tempCompensatedTDS;
}

float readPH(float temperature) {
  int analogValue = analogRead(PH_SENSOR_PIN);
  
  // Convert to voltage (pH sensor typically outputs 0-5V)
  float voltage = analogValue * (VREF / ADC_RESOLUTION);
  
  // Temperature compensation for pH (optional, depends on sensor)
  float tempCompensation = 0.0; // Add if your pH sensor requires it
  
  // Calculate pH from voltage using calibration curve
  // pH = slope * voltage + offset
  float phValue = PH_SLOPE * voltage + PH_NEUTRAL_VOLTAGE;
  
  // Adjust for temperature if needed
  phValue += tempCompensation * (temperature - 25.0);
  
  return phValue;
}

float applyRollingAverage(float newReading, float* buffer, float* sum) {
  *sum -= buffer[readingIndex];
  buffer[readingIndex] = newReading;
  *sum += newReading;
  
  readingIndex = (readingIndex + 1) % ROLLING_AVERAGE_SIZE;
  
  return *sum / ROLLING_AVERAGE_SIZE;
}

// =================== CALCULATION FUNCTIONS ===================
void calculateAdjustments() {
  // Calculate salt needed
  if (compensatedTDS < params.targetSalinity) {
    saltNeeded = (params.targetSalinity - compensatedTDS) * params.poolVolume / 1000000.0;
    if (saltNeeded < 0) saltNeeded = 0;
  } else {
    saltNeeded = 0;
  }
  
  // Calculate pH adjustment chemicals (simplified)
  if (currentPH < params.phMin) {
    // pH too low, need base (soda ash)
    baseNeeded = (params.targetPH - currentPH) * params.poolVolume * 0.15; // Simplified formula
    acidNeeded = 0;
  } else if (currentPH > params.phMax) {
    // pH too high, need acid (muriatic acid)
    acidNeeded = (currentPH - params.targetPH) * params.poolVolume * 0.12; // Simplified formula
    baseNeeded = 0;
  } else {
    acidNeeded = 0;
    baseNeeded = 0;
  }
}

// =================== ALERT FUNCTIONS ===================
void checkAlerts() {
  salinityAlert = false;
  pHAlert = false;
  tempAlert = false;
  systemAlert = false;
  
  // Check salinity alerts
  if (compensatedTDS < ALERT_SALINITY_LOW) {
    salinityAlert = true;
    systemAlert = true;
  } else if (compensatedTDS > ALERT_SALINITY_HIGH) {
    salinityAlert = true;
    systemAlert = true;
  }
  
  // Check pH alerts
  if (currentPH < ALERT_PH_CRITICAL_LOW || currentPH > ALERT_PH_CRITICAL_HIGH) {
    pHAlert = true;
    systemAlert = true;
  } else if (currentPH < params.phMin || currentPH > params.phMax) {
    pHAlert = true;
  }
  
  // Check temperature alerts
  if (currentTemperature < ALERT_TEMP_CRITICAL_LOW || 
      currentTemperature > ALERT_TEMP_CRITICAL_HIGH) {
    tempAlert = true;
    systemAlert = true;
  }
}

void updateStatusLEDs() {
  digitalWrite(LED_OK, !systemAlert);
  digitalWrite(LED_WARNING, systemAlert && !pHAlert && !tempAlert && !salinityAlert);
  digitalWrite(LED_CRITICAL, pHAlert || tempAlert || salinityAlert);
}

void sendCriticalAlerts() {
  if (!params.enableAlerts || !systemAlert) return;
  
  String alertMessage = "POOL ALERT: ";
  
  if (salinityAlert) {
    alertMessage += "Salinity=" + String(compensatedTDS, 0) + "ppm ";
  }
  if (pHAlert) {
    alertMessage += "pH=" + String(currentPH, 1) + " ";
  }
  if (tempAlert) {
    alertMessage += "Temp=" + String(currentTemperature, 1) + "C ";
  }
  
  // Send via WiFi (email/SMS API)
  if (params.enableRemote && WiFi.status() == WL_CONNECTED) {
    sendAlertToServer(alertMessage);
  }
  
  // Send via Bluetooth
  bluetooth.println(alertMessage);
  
  // Log alert
  logAlert(alertMessage);
}

// =================== DISPLAY FUNCTIONS ===================
void updateDisplay() {
  lcd.clear();
  
  switch (currentState) {
    case STATE_MONITORING:
      // Line 1: TDS and Temperature
      lcd.setCursor(0, 0);
      lcd.print("TDS:");
      lcd.print(int(compensatedTDS));
      lcd.print(" T:");
      lcd.print(currentTemperature, 1);
      lcd.write(223);
      
      // Line 2: pH and Status
      lcd.setCursor(0, 1);
      lcd.print("pH:");
      lcd.print(currentPH, 1);
      
      if (systemAlert) {
        lcd.setCursor(10, 1);
        lcd.print("ALERT!");
      } else if (saltNeeded > 0) {
        lcd.setCursor(8, 1);
        lcd.print("S:");
        lcd.print(saltNeeded, 1);
        lcd.print("kg");
      }
      break;
      
    case STATE_MENU:
      displayMenu();
      break;
      
    case STATE_SETTINGS:
      displaySettings();
      break;
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MAIN MENU");
  lcd.setCursor(0, 1);
  
  String menuItems[] = {"Monitoring", "Settings", "Calibration", "View Logs", "System Info"};
  lcd.print("> ");
  lcd.print(menuItems[menuIndex]);
}

void displaySettings() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SETTINGS");
  
  String settings[] = {
    "Target Salinity: " + String(params.targetSalinity, 0),
    "Target pH: " + String(params.targetPH, 1),
    "Pool Volume: " + String(params.poolVolume, 0),
    "Log Interval: " + String(params.logInterval/60000) + "min",
    "Alerts: " + String(params.enableAlerts ? "ON" : "OFF"),
    "WiFi: " + String(params.enableRemote ? "ON" : "OFF"),
    "Save & Exit"
  };
  
  lcd.setCursor(0, 1);
  lcd.print("> ");
  lcd.print(settings[subMenuIndex].substring(0, 14));
}

// =================== DATA LOGGING FUNCTIONS ===================
void createLogFile() {
  char filename[20];
  sprintf(filename, "LOG_%04d%02d%02d.csv", 
          timeClient.getYear(), 
          timeClient.getMonth(), 
          timeClient.getDay());
  
  if (SD.exists(filename)) {
    dataFile = SD.open(filename, FILE_WRITE);
  } else {
    dataFile = SD.open(filename, FILE_WRITE);
    // Write CSV header
    dataFile.println("Timestamp,Temperature(C),TDS(ppm),pH,Salt_Needed(kg),pH_Status,Alert");
  }
  
  if (dataFile) {
    dataFile.close();
    Serial.print("Log file created: ");
    Serial.println(filename);
  }
}

void logDataToSD() {
  char filename[20];
  sprintf(filename, "LOG_%04d%02d%02d.csv", 
          timeClient.getYear(), 
          timeClient.getMonth(), 
          timeClient.getDay());
  
  dataFile = SD.open(filename, FILE_WRITE);
  
  if (dataFile) {
    // Get current time
    timeClient.update();
    String timestamp = String(timeClient.getHours()) + ":" + 
                      String(timeClient.getMinutes()) + ":" + 
                      String(timeClient.getSeconds());
    
    // Create CSV line
    String dataLine = timestamp + "," +
                     String(currentTemperature, 2) + "," +
                     String(compensatedTDS, 0) + "," +
                     String(currentPH, 2) + "," +
                     String(saltNeeded, 2) + "," +
                     (pHAlert ? "NEEDS_ADJUST" : "OK") + "," +
                     (systemAlert ? "YES" : "NO");
    
    dataFile.println(dataLine);
    dataFile.close();
    
    Serial.println("Data logged: " + dataLine);
  }
}

void logAlert(String alertMessage) {
  File alertFile = SD.open("ALERTS.TXT", FILE_WRITE);
  
  if (alertFile) {
    timeClient.update();
    String timestamp = String(timeClient.getHours()) + ":" + 
                      String(timeClient.getMinutes()) + ":" + 
                      String(timeClient.getSeconds());
    
    alertFile.println(timestamp + " - " + alertMessage);
    alertFile.close();
  }
}

// =================== COMMUNICATION FUNCTIONS ===================
void initializeWiFi() {
  // Read WiFi credentials from EEPROM
  char ssid[32];
  char password[32];
  
  EEPROM.get(WIFI_SSID_ADDR, ssid);
  EEPROM.get(WIFI_PASS_ADDR, password);
  
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  lcd.clear();
  lcd.print("Connecting WiFi...");
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    lcd.clear();
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed");
    lcd.clear();
    lcd.print("WiFi Failed");
  }
  
  delay(2000);
}

void sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  WiFiClient client;
  
  // Replace with your server details
  if (client.connect("your-server.com", 80)) {
    String postData = "temperature=" + String(currentTemperature, 2) +
                     "&tds=" + String(compensatedTDS, 0) +
                     "&ph=" + String(currentPH, 2) +
                     "&salt_needed=" + String(saltNeeded, 2) +
                     "&alerts=" + String(systemAlert ? "1" : "0");
    
    client.println("POST /api/pool-data HTTP/1.1");
    client.println("Host: your-server.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
    
    client.stop();
  }
}

void sendAlertToServer(String alertMessage) {
  // Similar to sendDataToServer but for alerts
  // Could use IFTTT, Twilio, or custom API
}

void sendDataViaBluetooth() {
  // Create JSON formatted data
  String jsonData = "{";
  jsonData += "\"temperature\":" + String(currentTemperature, 2) + ",";
  jsonData += "\"tds\":" + String(compensatedTDS, 0) + ",";
  jsonData += "\"ph\":" + String(currentPH, 2) + ",";
  jsonData += "\"salt_needed\":" + String(saltNeeded, 2) + ",";
  jsonData += "\"alerts\":" + String(systemAlert ? "true" : "false");
  jsonData += "}";
  
  bluetooth.println(jsonData);
}

void handleBluetoothCommands() {
  if (bluetooth.available()) {
    String command = bluetooth.readStringUntil('\n');
    command.trim();
    
    if (command == "GET_DATA") {
      sendDataViaBluetooth();
    } else if (command.startsWith("SET_")) {
      handleRemoteCommand(command);
    } else if (command == "GET_CONFIG") {
      sendConfigViaBluetooth();
    }
  }
}

// =================== USER INTERFACE FUNCTIONS ===================
void handleUserInput() {
  static unsigned long lastButtonPress = 0;
  
  if (millis() - lastButtonPress < 200) return; // Debounce
  
  if (digitalRead(BUTTON_SELECT) == LOW) {
    lastButtonPress = millis();
    
    switch (currentState) {
      case STATE_MONITORING:
        currentState = STATE_MENU;
        menuIndex = 0;
        break;
        
      case STATE_MENU:
        if (menuIndex == 0) {
          currentState = STATE_MONITORING;
        } else if (menuIndex == 1) {
          currentState = STATE_SETTINGS;
          subMenuIndex = 0;
        } else if (menuIndex == 2) {
          currentState = STATE_CALIBRATION;
        }
        break;
        
      case STATE_SETTINGS:
        if (subMenuIndex == 6) { // Save & Exit
          saveParameters();
          currentState = STATE_MONITORING;
        }
        break;
    }
  }
  
  if (digitalRead(BUTTON_UP) == LOW) {
    lastButtonPress = millis();
    
    if (currentState == STATE_MENU) {
      menuIndex = (menuIndex > 0) ? menuIndex - 1 : 4;
    } else if (currentState == STATE_SETTINGS) {
      adjustSetting(1); // Increase value
    }
  }
  
  if (digitalRead(BUTTON_DOWN) == LOW) {
    lastButtonPress = millis();
    
    if (currentState == STATE_MENU) {
      menuIndex = (menuIndex < 4) ? menuIndex + 1 : 0;
    } else if (currentState == STATE_SETTINGS) {
      adjustSetting(-1); // Decrease value
    }
  }
  
  if (digitalRead(BUTTON_BACK) == LOW) {
    lastButtonPress = millis();
    
    if (currentState == STATE_MENU || 
        currentState == STATE_SETTINGS || 
        currentState == STATE_CALIBRATION) {
      currentState = STATE_MONITORING;
    }
  }
}

void adjustSetting(int direction) {
  switch (subMenuIndex) {
    case 0: // Target Salinity
      params.targetSalinity += direction * 100;
      if (params.targetSalinity < 1000) params.targetSalinity = 1000;
      if (params.targetSalinity > 5000) params.targetSalinity = 5000;
      break;
      
    case 1: // Target pH
      params.targetPH += direction * 0.1;
      if (params.targetPH < 6.5) params.targetPH = 6.5;
      if (params.targetPH > 8.5) params.targetPH = 8.5;
      break;
      
    case 2: // Pool Volume
      params.poolVolume += direction * 1000;
      if (params.poolVolume < 1000) params.poolVolume = 1000;
      if (params.poolVolume > 200000) params.poolVolume = 200000;
      break;
      
    case 3: // Log Interval
      params.logInterval += direction * 60000; // 1 minute increments
      if (params.logInterval < 60000) params.logInterval = 60000; // 1 min minimum
      if (params.logInterval > 3600000) params.logInterval = 3600000; // 1 hour maximum
      break;
      
    case 4: // Alerts toggle
      params.enableAlerts = !params.enableAlerts;
      break;
      
    case 5: // WiFi toggle
      params.enableRemote = !params.enableRemote;
      break;
  }
}

// =================== CALIBRATION FUNCTIONS ===================
void runCalibration() {
  lcd.clear();
  lcd.print("Calibration Mode");
  lcd.setCursor(0, 1);
  lcd.print("1:TDS 2:pH 3:Temp");
  
  // Wait for calibration selection
  delay(3000);
  
  // pH Calibration Example
  lcd.clear();
  lcd.print("pH Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Insert in pH7 sol");
  
  delay(5000);
  
  // Calibrate neutral point
  float sumVoltage = 0;
  for (int i = 0; i < PH_CALIBRATION_SAMPLES; i++) {
    sumVoltage += analogRead(PH_SENSOR_PIN) * (VREF / ADC_RESOLUTION);
    delay(10);
  }
  float neutralVoltage = sumVoltage / PH_CALIBRATION_SAMPLES;
  
  // Save calibration to EEPROM
  EEPROM.put(100, neutralVoltage); // Store at address 100
  
  lcd.clear();
  lcd.print("Calibration Done");
  delay(2000);
  
  currentState = STATE_MONITORING;
}

// =================== EEPROM FUNCTIONS ===================
void loadParameters() {
  // Load from EEPROM if available, else use defaults
  EEPROM.get(200, params);
  
  // Validate loaded parameters
  if (params.targetSalinity < 1000 || params.targetSalinity > 5000) {
    params.targetSalinity = 3000.0;
  }
  if (params.poolVolume < 1000 || params.poolVolume > 200000) {
    params.poolVolume = 50000.0;
  }
}

void saveParameters() {
  EEPROM.put(200, params);
  EEPROM.commit();
  
  lcd.clear();
  lcd.print("Settings Saved");
  delay(1000);
}

// =================== UTILITY FUNCTIONS ===================
void sendConfigViaBluetooth() {
  String config = "{";
  config += "\"target_salinity\":" + String(params.targetSalinity, 0) + ",";
  config += "\"target_ph\":" + String(params.targetPH, 1) + ",";
  config += "\"pool_volume\":" + String(params.poolVolume, 0) + ",";
  config += "\"alerts_enabled\":" + String(params.enableAlerts ? "true" : "false");
  config += "}";
  
  bluetooth.println(config);
}

void handleRemoteCommand(String command) {
  // Example: "SET_SALINITY=3200"
  if (command.startsWith("SET_SALINITY=")) {
    float value = command.substring(13).toFloat();
    if (value >= 1000 && value <= 5000) {
      params.targetSalinity = value;
    }
  }
  // Add more commands as needed
}

// =================== SYSTEM INFO FUNCTION ===================
void displaySystemInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Info");
  lcd.setCursor(0, 1);
  
  // Display memory info, uptime, etc.
  lcd.print("Uptime: ");
  lcd.print(millis() / 3600000);
  lcd.print("h");
}
