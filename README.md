# 🛡️ Smart Helmet for Rider Safety | ESP32-Based IoT System

A final year engineering project designed to enhance road safety for two-wheeler riders using real-time health monitoring, accident detection, and emergency alert systems — all powered by the ESP32 microcontroller.

---

## 🚀 Features

- ✅ **Heart Rate & SpO₂ Monitoring** — Using MAX30105 sensor
- ✅ **Fall Detection** — MPU6050 (Accelerometer + Gyroscope)
- ✅ **Alcohol Detection** — MQ3 gas sensor to detect alcohol consumption
- ✅ **Helmet Wear Detection** — Infrared (IR) sensor
- ✅ **Emergency Alerts via SMS** — SIM800L GSM module
- ✅ **Live GPS Tracking** — Location sent via Google Maps link
- ✅ **Real-Time Data Plotting** — Heart rate, SpO₂, and EEG simulation using MATLAB

---

## ⚙️ Hardware Components

| Component        | Description                                |
|------------------|--------------------------------------------|
| ESP32            | Main microcontroller with Wi-Fi & BLE      |
| MAX30105         | Heart Rate & SpO₂ Sensor                   |
| MPU6050          | Fall Detection (Accelerometer + Gyro)     |
| MQ3              | Alcohol detection sensor                   |
| IR Sensor        | Helmet wear detection                      |
| SIM800L          | GSM Module for SMS                         |
| TinyGPS++        | For parsing GPS location                   |
| Buzzer           | Warning alert system                       |

---

## 📡 Working Principle

1. Rider wears the helmet → IR sensor confirms helmet is worn.
2. Monitors vitals continuously using MAX30105.
3. Detects alcohol content via MQ3.
4. MPU6050 senses abnormal motion (falls or accidents).
5. If an accident is detected:
   - System checks vitals and alcohol status.
   - Sends emergency SMS with:
     - Rider’s condition (fine/not fine)
     - Heart Rate & SpO₂
     - Location as a Google Maps link
6. All vitals can be visualized in **MATLAB** using live plots.

---

## 🧠 Software & Tools

- **Arduino IDE** (ESP32 Programming)
- **MATLAB** (Live Data Visualization)
- **Libraries Used:**
  - `TinyGPS++`
  - `Adafruit_Sensor`
  - `Wire.h`, `SoftwareSerial`, etc.

---


---

## 🙏 Acknowledgements

Special thanks to our guide Prof.Agi George for constant support and technical mentorship throughout this project.

---

## 📁 Project Structure

```bash
SmartHelmetProject/
├── SmartHelmet.ino         # Main ESP32 code
├── MATLAB/
│   └── live_plot.m         # Real-time plots for Heart Rate, SpO2, EEG
├── Images/
│   └── circuit_diagram.png
└── README.md               # This file
