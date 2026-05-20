# 🛡️ IoT Industrial Security Matrix (Protection System)

A highly responsive, multi-layered IoT security system designed for industrial environments and vaults. This project bridges physical hardware responses with continuous cloud monitoring without any blocking delays.

## 👨‍💻 Author
**Jimmy Hadad** 

## 🎯 Project Objective
To design and implement a security system that detects, prevents, and reports unauthorized access in real-time. By utilizing **FreeRTOS** on the ESP32, the system ensures instantaneous security reactions locally while simultaneously routing actionable data to a Telegram Bot via Google Apps Script.

## ⚙️ Hardware Components
* **Microcontroller:** ESP32 Dual-Core
* **User Interface:** 0.96" OLED Display & 4x4 Matrix Keypad
* **Audio Feedback:** DFRobot DFPlayer Mini MP3 Module
* **Physical Defense:** 5V Relay Module (for external alarms/locks)
* **Status Indicators:** RGB LED & Active Buzzer

## 🚀 Key Features
* **Multi-Level Access:** Supports both Master (`2005`) and User (`1234`) privilege codes.
* **Zero-Delay IoT (Multithreading):** Uses FreeRTOS background tasks to handle Wi-Fi and HTTPS requests, ensuring the main security loop never freezes.
* **System Lockdown Protocol:** Automatically triggers after 3 consecutive failed attempts. It locks the keypad, activates the relay, and enters a permanent hardware lock loop until a manual reset.
* **Cloud Reporting:** Secured Webhook interface with Google Apps Script to bypass standard ESP32 HTTPS limitations, delivering instant push notifications via Telegram.

## 📂 Repository Structure
* `ESP32_Security_Matrix.ino` : The main C++ code for the ESP32 microcontroller.
* `Telegram_Bot_Script.gs` : The JavaScript code to be deployed on Google Apps Script as a Web App to handle Telegram Webhooks.

## ⚠️ Setup & Deployment
1. **Google Apps Script:** Create a new project, paste the `.gs` code, insert your Telegram Bot Token, deploy as a Web App, and copy the Web App URL.
2. **Webhook:** Set your Telegram bot webhook to point to the Google Apps Script URL.
3. **ESP32:** Update the Wi-Fi credentials (`ssid`, `password`) and the `scriptUrl` in the `.ino` file before uploading to the board.
