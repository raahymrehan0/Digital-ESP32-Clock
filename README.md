# About my Project

#### A C++ script for the ESP32 microcontroller that connects to Wi-Fi to retrieve and display real-time data. Utilizes ESP32 libraries to fetch the current time and render a digital clock interface on the LilyGO TTGO display.

## Functionality

#### The script connects the ESP32 microcontroller to an available Wi-Fi network. Simply enter your SSID, username, and password. It includes specific code for connecting to an eduroam network. Once connected, the ESP32 uses the Network Time Protocol (NTP) to retrieve the current time from an online server, set by default to British Summer Time but adjustable for other time zones by modifying the timeClient parameters (line 24). The time updates every second, displayed in the serial monitor, and also shown as a digital clock on the LilyGO TTGO display using graphics libraries.
