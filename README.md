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

## Author

MD Hashim  
B.Tech – Mechanical Engineering  
Embedded Systems and IoT Projects
---
## License
Apache License 2.0
