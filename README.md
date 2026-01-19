# Advanced-smart-pool-water-quality-monitoring
Advanced Arduino-based smart pool water quality monitoring system with TDS, pH, temperature sensing, data logging, WiFi/Bluetooth connectivity, and intelligent alerts.
# Advanced Smart Pool Water Quality Monitoring System

An Arduino-based embedded system for real-time monitoring and management of swimming pool water quality.  
The system measures TDS (salinity), pH, and water temperature, logs data to an SD card, and supports WiFi/Bluetooth-based remote monitoring and alerts.

---

## Overview

Maintaining correct pool water chemistry is critical for safety, comfort, and equipment life.  
This project automates water quality monitoring using sensors, local processing, and optional IoT connectivity.

The system is modular, configurable, and suitable for both residential and small commercial pools.

---

## Main Capabilities

- Continuous monitoring of TDS, pH, and water temperature
- Temperature-compensated TDS calculation
- Rolling-average filtering for stable sensor readings
- Automatic calculation of salt and pH correction requirements
- SD card data logging with timestamps
- WiFi-based remote monitoring
- Bluetooth data streaming for mobile applications
- Configurable alert thresholds and status indicators
- LCD-based local user interface with buttons

---

## Hardware Summary

- Arduino Uno / Mega / ESP32
- TDS sensor (analog)
- pH sensor (analog, calibrated)
- DS18B20 digital temperature sensor
- 16x2 I2C LCD
- SD card module (SPI)
- WiFi and/or Bluetooth module
- Push buttons and status LEDs
---
## Hardware Connections

Arduino Uno Pinout:
• A0: TDS Sensor output
• A1: (Optional) Spare analog input
• A2: pH Sensor output
• D2: OneWire for DS18B20
• D3-D6: Buttons (Up, Down, Select, Back)
• D7-D9: Status LEDs (Green, Yellow, Red)
• D10: SD Card CS
• D11: SD Card MOSI
• D12: SD Card MISO
• D13: SD Card SCK
• D14(A0)-D15(A1): Bluetooth TX/RX
• 5V: Sensors, SD Card, LCD
• GND: Common ground

---

---

## System Flow

Sensors → Microcontroller → Processing  
→ LCD Display  
→ SD Card Logging  
→ WiFi / Bluetooth  
→ Alerts

---

## Firmware Highlights

- State-machine based user interface
- Non-blocking timing using millis()
- EEPROM-backed configuration storage
- Modular functions for sensing, logging, alerts, and communication
- Expandable for cloud services and mobile apps

---
┌─────────────────────────────────────────────────────────┐
│                    SMART POOL SYSTEM                    │
├─────────────────────────────────────────────────────────┤
│ Sensors:                                                │
│  • TDS Sensor → Conductivity → Temperature Compensation │
│  • pH Sensor → Voltage → Calibration Curve              │
│  • DS18B20 → Digital Temperature                        │
├─────────────────────────────────────────────────────────┤
│ Microcontroller:                                        │
│  • Arduino Uno (or Mega for more pins)                  │
│  • Analog Pins: TDS(A0), pH(A2), Temp(OneWire)          │
│  • Digital Pins: Buttons, LEDs, SD, Bluetooth           │
├─────────────────────────────────────────────────────────┤
│ Data Flow:                                              │
│  Sensors → Arduino → Processing → Multiple Outputs      │
│     ↓        ↓         ↓                                 │
│     SD Card  LCD      Alerts                            │
│     ↓        ↓         ↓                                 │
│     Logs   Display  Email/SMS                           │
│     ↓                   ↓                                │
│  Historical Data   Remote Monitoring                    │
└─────────────────────────────────────────────────────────┘
---

## Installation

1. Connect sensors and modules according to pin definitions in the firmware
2. Install required Arduino libraries
3. Insert a FAT32-formatted SD card
4. Upload the firmware to the board
5. Power on the system
6. Configure parameters via LCD menu
7. Calibrate sensors before operation

---

## Calibration

pH calibration is performed using standard buffer solutions (pH 4, 7, and 10).  
TDS calibration is performed using a known conductivity solution.  
Calibration values are stored in EEPROM.

---

## Data Logging

- Daily CSV log files stored on the SD card
- Timestamped sensor readings
- Alert events logged separately

---

## Remote Monitoring

- WiFi support for server or cloud-based monitoring
- Bluetooth JSON data stream for mobile applications
---
## For more performance and Display capability use ESP32 - P4
# ESP32-P4 Smart Pool Water Quality Monitoring System

## 📋 Project Overview
A comprehensive IoT-based water quality monitoring system for swimming pools using ESP32-P4 microcontroller with TDS, pH, and temperature sensors. The system calculates chemical adjustments needed and provides remote monitoring capabilities.

## 🎯 Features
- ✅ **Multi-parameter Monitoring**: TDS, pH, and Temperature
- ✅ **Temperature Compensation**: Accurate TDS readings
- ✅ **Chemical Calculation**: Salt, acid, and base requirements
- ✅ **Data Logging**: SD card storage with timestamps
- ✅ **Remote Monitoring**: WiFi and Bluetooth connectivity
- ✅ **Alert System**: Email/SMS notifications for critical conditions
- ✅ **User Interface**: LCD display with button controls
- ✅ **Web Interface**: Real-time data visualization

## 🛠️ Hardware Requirements

### Microcontroller
- **ESP32-P4** (or compatible ESP32 development board)

### Sensors
1. **TDS Sensor** (Keyestudio/DFRobot analog output)
2. **pH Sensor** (Analog pH meter with BNC connector)
3. **Temperature Sensor** (DS18B20 waterproof)
4. **Optional**: ORP, Chlorine sensors

### Modules
1. **SD Card Module** (SPI interface)
2. **16x2 I2C LCD Display**
3. **4-channel Logic Level Converter** (5V to 3.3V)
4. **Buttons** (x4 for UI control)
5. **LEDs** (x3 for status indication)

### Power
- 5V/3A Power Supply
- Voltage Regulators (if needed)
- 100μF and 0.1μF capacitors for stability

## 📍 ESP32-P4 Pin Connections

### Sensor Connections
| ESP32-P4 Pin | Function | Connection | Notes |
|-------------|----------|------------|-------|
| **GPIO1** | TDS Sensor | Analog Output | Via voltage divider |
| **GPIO3** | pH Sensor | Analog Output | Via voltage divider |
| **GPIO7** | DS18B20 | OneWire Data | 4.7kΩ pull-up to 3.3V |
| **GND** | All Sensors | Ground | Common ground |
| **3.3V** | DS18B20 | Power | Direct connection |
| **5V** | TDS/pH Sensors | Power | Via level shifter |

### User Interface
| ESP32-P4 Pin | Component | Function |
|-------------|-----------|----------|
| **GPIO11** | Button | UP |
| **GPIO12** | Button | DOWN |
| **GPIO13** | Button | SELECT |
| **GPIO14** | Button | BACK |
| **GPIO15** | LED | Green (OK) |
| **GPIO16** | LED | Yellow (Warning) |
| **GPIO17** | LED | Red (Critical) |

### SD Card Module (VSPI)
| ESP32-P4 Pin | SD Card Pin | Function |
|-------------|------------|----------|
| **GPIO18** | CS | Chip Select |
| **GPIO23** | MOSI | Master Out Slave In |
| **GPIO19** | MISO | Master In Slave Out |
| **GPIO5** | SCK | Serial Clock |
| **3.3V** | VCC | Power |
| **GND** | GND | Ground |

### I2C LCD Display
| ESP32-P4 Pin | LCD Pin | Function |
|-------------|---------|----------|
| **GPIO21** | SDA | I2C Data |
| **GPIO22** | SCL | I2C Clock |
| **3.3V** | VCC | Power |
| **GND** | GND | Ground |

## ⚡ Voltage Level Conversion (CRITICAL)

### For 5V Sensors to ESP32-P4 (3.3V)
TDS/pH Sensor Output (0-5V)
│
├─── 10kΩ Resistor ────┐
│ │
└─── 20kΩ Resistor ────┼─── ESP32-P4 GPIO (0-3.3V)
│
GND

text

**Calculation**: Voltage divider ratio = 20k/(10k+20k) = 2/3 ≈ 5V→3.3V

### Alternative: Use Logic Level Converter
- Connect 5V side to sensor output
- Connect 3.3V side to ESP32-P4 GPIO
- Ensure proper direction (uni/bidirectional as needed)

## 🔧 Software Setup

### Prerequisites
- Arduino IDE 2.0 or later
- ESP32 Board Support Package
- Required Libraries (see below)

### Installation Steps

1. **Install ESP32 Board Support**
File → Preferences → Additional Boards Manager URLs
Add: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Tools → Board → Boards Manager → Search "ESP32" → Install

text

2. **Select Board Configuration**
Board: ESP32-P4 (or generic ESP32 Dev Module)
Upload Speed: 921600
Flash Size: 8MB
Partition Scheme: Default 8MB with SPIFFS

text

3. **Install Required Libraries**
```cpp
// Install via Library Manager:
- Wire.h (built-in)
- LiquidCrystal_I2C.h
- SD.h (built-in)
- SPI.h (built-in)
- WiFi.h (built-in)
- BluetoothSerial.h (built-in)
- OneWire.h
- DallasTemperature.h
- NTPClient.h
- EEPROM.h (built-in)
- ArduinoJson.h
Upload Code

Connect ESP32-P4 via USB

Select correct COM port

Upload the provided .ino file

📁 Project Structure
text
smart-pool-monitor/
├── src/
│   ├── main.ino              # Main program
│   ├── sensors.cpp           # Sensor reading functions
│   ├── calculations.cpp      # Chemical calculations
│   ├── display.cpp           # LCD and UI functions
│   ├── logging.cpp           # SD card operations
│   ├── communication.cpp     # WiFi/Bluetooth functions
│   └── alerts.cpp            # Alert system
├── lib/                      # Custom libraries
├── data/                     # Web interface files
├── logs/                     # Sample log files
├── schematics/               # Circuit diagrams
└── README.md                 # This file
⚙️ Configuration
Initial Setup
pH Sensor Calibration

cpp
// Required buffer solutions:
// 1. pH 4.0
// 2. pH 7.0
// 3. pH 10.0

// Run calibration from menu:
// Settings → Calibration → pH Sensor
WiFi Configuration

cpp
// Method 1: Hardcode in setup()
const char* ssid = "YourWiFi";
const char* password = "YourPassword";

// Method 2: Use config file on SD card
// Create config.txt with:
// SSID=YourWiFi
// PASSWORD=YourPassword
// API_KEY=YourAPIKey
Pool Parameters

cpp
// Default values (adjust via UI):
Target Salinity: 3000 ppm
Target pH: 7.4
Pool Volume: 50000 liters
Log Interval: 5 minutes
Alert Thresholds: Customizable
🔌 Wiring Diagram
text
┌─────────────────────────────────────────────────────────────┐
│                         ESP32-P4                            │
├─────────────────────────────────────────────────────────────┤
│ POWER:                                                      │
│   3.3V ──┬──▶ LCD VCC                                       │
│          ├──▶ SD Card VCC                                   │
│          └──▶ DS18B20 VCC                                   │
│                                                             │
│   5V ────┼──▶ Level Shifter HV (for TDS/pH sensors)        │
│                                                             │
│   GND ───┼──▶ All components ground                         │
├─────────────────────────────────────────────────────────────┤
│ SENSORS (via Level Shifter):                                │
│   TDS Out ───▶ LV1 ───▶ GPIO1                               │
│   pH Out ────▶ LV2 ───▶ GPIO3                               │
│   DS18B20 ──────────────────────▶ GPIO7 + 4.7kΩ to 3.3V    │
├─────────────────────────────────────────────────────────────┤
│ USER INTERFACE:                                             │
│   Buttons ────┬──▶ GPIO11-14 (with internal pull-up)       │
│   LEDs ───────┼──▶ GPIO15-17 (220Ω resistor each)          │
├─────────────────────────────────────────────────────────────┤
│ SD CARD:                                                    │
│   CS ────────▶ GPIO18                                       │
│   MOSI ──────▶ GPIO23                                       │
│   MISO ──────▶ GPIO19                                       │
│   SCK ───────▶ GPIO5                                        │
├─────────────────────────────────────────────────────────────┤
│ I2C LCD:                                                    │
│   SDA ───────▶ GPIO21                                       │
│   SCL ───────▶ GPIO22                                       │
└─────────────────────────────────────────────────────────────┘
🌐 Web Interface Setup
Option 1: Built-in Web Server
cpp
// Enable in code:
// #define ENABLE_WEB_SERVER true

// Access at: http://[ESP32-IP-ADDRESS]
Option 2: Cloud Integration (Thingspeak)
Create account at thingspeak.com

Create new channel

Update API key in code:

cpp
const char* thingspeakApiKey = "YOUR_API_KEY";
Option 3: Custom API
cpp
// Modify sendDataToServer() function
const char* server = "your-server.com";
const char* apiKey = "your-api-key";
📊 Data Log Format
SD card creates CSV files with naming: LOG_YYYYMMDD.csv

csv
Timestamp,Temperature(C),TDS(ppm),pH,Salt_Needed(kg),pH_Status,Alert
14:30:45,28.5,2850,7.3,7.5,NEEDS_ADJUST,NO
14:35:45,28.6,2860,7.4,6.8,OK,NO
14:40:45,28.7,2870,7.5,5.2,OK,NO
Alerts are logged in: ALERTS.TXT

🔔 Alert Configuration
Alert Types
Critical Alerts (Immediate notification)

pH < 6.8 or > 8.0

Temperature < 10°C or > 40°C

TDS < 2000 or > 4000 ppm

Warning Alerts (Display only)

pH < 7.2 or > 7.8

TDS < 2500 or > 3500 ppm

Alert Methods
LCD display message

Status LED indication

Email notification (via IFTTT/Webhook)

SMS alert (via Twilio)

Bluetooth notification to paired device

🔋 Power Management
Power Saving Features
cpp
// Enable deep sleep between readings
#define DEEP_SLEEP_ENABLED false  // Set true for battery operation

// Deep sleep duration (seconds)
#define SLEEP_DURATION 300  // 5 minutes

// Wake sources:
// 1. Timer (automatic wake)
// 2. External interrupt (button press)
// 3. UART (Bluetooth/WiFi)
Power Consumption Estimates
Mode	Current	Duration/Day	Energy
Active	120mA	1 hour	0.6Ah
Light Sleep	20mA	23 hours	0.46Ah
Deep Sleep	0.1mA	24 hours	0.0024Ah
Total	-	24 hours	~1.06Ah
Battery Recommendation: 12V 7Ah lead-acid or 3.7V 3000mAh Li-ion

🚀 Advanced Features
1. Predictive Maintenance
cpp
// Track sensor drift over time
void checkSensorHealth() {
  // Monitor baseline changes
  // Alert if calibration needed
  // Log maintenance schedule
}
2. Automated Chemical Dosing
cpp
// Integration with peristaltic pumps
void controlChemicalPump(int pumpPin, float mlToDose) {
  // Calculate pump runtime
  // Activate relay/pump driver
  // Monitor flow rate
}
3. Weather Integration
cpp
// Adjust calculations based on weather
void adjustForWeather() {
  // Consider rainfall (dilution)
  // Consider sunlight (chlorine degradation)
  // Consider temperature changes
}
🐛 Troubleshooting
Common Issues
Problem	Symptoms	Solution
No sensor readings	LCD shows 0 values	Check voltage dividers, verify connections
WiFi disconnects frequently	Random disconnections	Check power supply stability, add capacitors
SD card errors	"SD init failed"	Format SD card as FAT32, check wiring
pH readings unstable	Fluctuating values	Calibrate sensor, check probe cleanliness
ESP32-P4 not programming	Upload fails	Check USB cable, drivers, boot mode
Debug Commands via Serial Monitor
bash
# Available commands:
# GET_DATA     - Display current readings
# GET_CONFIG   - Show current configuration
# SET_VOLUME=50000  - Set pool volume
# SET_SALINITY=3000 - Set target salinity
# CALIBRATE_PH - Start pH calibration
# RESET        - Factory reset
# HELP         - Show all commands
📈 Performance Metrics
Reading Accuracy
Temperature: ±0.5°C

TDS: ±10% (after calibration)

pH: ±0.1 pH (with regular calibration)

Response Time
Sensor reading: 2 seconds

Data logging: < 1 second

Alert generation: < 5 seconds

Web interface update: 10 seconds interval

🔒 Safety Notes
Electrical Safety

Use waterproof enclosures for outdoor installation

Install GFCI protection for all electrical connections

Properly ground all metal components

Chemical Safety

Never mix chemicals directly

Wear protective gear when handling chemicals

Store chemicals away from electronic components

Water Safety

Regular sensor calibration for accurate readings

Manual verification of chemical levels recommended

System is an aid, not replacement for professional testing
---

## Typical Applications

- Residential swimming pools
- Commercial pool facilities
- Smart water quality management systems
- Embedded systems and IoT portfolios

---

## Safety and Maintenance

- Use waterproof enclosures for electronics
- Regularly clean and calibrate sensors
- Ensure stable power supply
- Keep pool chemicals isolated from electronics

---
🙏 Acknowledgments
Espressif Systems for ESP32-P4

Keyestudio for TDS sensor design

Open-source community for libraries

Contributors and testers
---

## Author

MD Hashim  
B.Tech – Mechanical Engineering  
Embedded Systems and IoT Projects
---
## License
Apache License 2.0
