<p align="center">
  <img src="./img.png" alt="Project Banner" width="100%">
</p>

# RAKSHAK-1.0🎯

## Basic Details

### Team Name: META MAKERS

### Team Members
- Member 1: Aswana N - NSS COLLEGE OF ENGINEERING , PALAKKAD

### Hosted Project Link
1. https://drive.google.com/file/d/1A8QwXHA24_kf9ueLwoiRuupRxyYLmEjs/view?usp=drivesdk <br>
2. https://drive.google.com/file/d/1H0Yil8FEZiCKtIFs6si4XcBkV7o9oqfa/view?usp=drivesdk

### Project Description
Rakshak 1.0 is a LoRa-based women safety system designed for rural areas that works without internet connectivity. It instantly transmits emergency alerts with live GPS location through panic activation or automatic fall detection to nearby police stations for rapid response.

### The Problem statement
Women in rural areas lack reliable emergency communication due to poor internet connectivity, making existing safety solutions ineffective during critical situations.

### The Solution
We solve this problem by developing Rakshak 1.0, a wearable, offline women safety system that uses long-range, low-power LoRa communication instead of internet-based networks. LoRa technology enables reliable transmission over 5–10 km while consuming minimal power, making the device suitable for battery-operated wearable applications. The system integrates an ESP32 microcontroller, GPS module, accelerometer, panic button, and cancel mechanism to detect emergencies either manually or automatically through fall detection. Upon activation, it transmits real-time latitude, longitude, device ID, and alert type directly to a LoRa receiver installed at a police station. The receiver system processes the data using a Python-based monitoring interface and displays the exact location on a digital map, enabling authorities to respond quickly even in network-deficient rural environments.
![Architecture](Images/Architecture.png)

## Technical Details

### Technologies/Components Used

**For Software:**
- Languages used: Python
- Frameworks used: None
- Libraries used: Pyserial,Requests,Plyer
- Tools used: Python,Notepad,Web browser

**For Hardware:**
- Main components: ESP 32,NEO 6M GPS Module,RA-02 LoRa Module,OLED Display,ADXL335 Accelerometer
- Tools required: USB Cable,Breadboard,Jumper wires/Single strand wires,PC etc....

---

## Features

List the key features of your project:
- Feature 1: Internet-Independent Long-Range Communication.
- Feature 2: Real-Time Panic and Fall Detection.
- Feature 3: Unique Device ID-Based User Identification.
- Feature 4: Offline Monitoring and Alert System.


## Implementation

### For Software:

#### Installation
```bash
[Installation commands - pip install pyserial plyer requests]
```

#### Run
```bash
[Run commands - python rakshak_monitor.py]
```

### For Hardware:

#### Components Required
1. ESP32 Microcontroller  
2. LoRa Module (433 MHz)  
3. GPS Module (NEO-6M)  
4. Accelerometer  
5. OLED Display  
6. Push Buttons  
7. LoRa Antenna  
8. Connecting Wires  

#### Circuit Setup
The transmitter circuit is built around the ESP32 microcontroller, where the LoRa module is connected via SPI communication pins, the GPS module through UART, and the accelerometer and OLED display through I2C interface. Panic and cancel buttons are connected to digital GPIO pins with pull-up configuration for emergency triggering. The receiver side uses another ESP32 interfaced with a LoRa module via SPI and connected to a computer through USB for data visualization.

![Workflow](Images/Workflow.png)

---

## Project Documentation

### For Software:

#### Screenshots (Add at least 3)

![Screenshot1](Images/Monitor.png)
Shows the Rakshak 1.0 Python-based monitoring application running in Windows PowerShell, continuously listening to the ESP32 receiver for incoming LoRa emergency data in real time.

![Screenshot2](Images/Map.png)
Displays the automatically opened OpenStreetMap view pinpointing the user’s emergency location using received latitude and longitude coordinates for quick visual identification.

![Screenshot3](Images/Alert_msg.png)
Shows the processed emergency message including device ID, user name, fallback GPS handling, precise coordinates, human-readable location, and confirmation of panic alert detection.

#### Diagrams

**System Architecture:**

![Architecture Diagram](docs/Workflow.png)

**Application Workflow:**

![Workflow](docs/workflow.png)
*Add caption explaining your workflow*

---

### For Hardware:

#### Schematic & Circuit

![Circuit](Images/ckt_dgm.png)
This schematic illustrates the complete hardware interconnection of the Rakshak 1.0 women safety system. The transmitter ESP32 interfaces with the NEO-6M GPS module via UART for real-time location acquisition, the ADXL335 accelerometer through analog GPIO pins for fall detection, and an OLED display using I²C for status visualization. Emergency triggers are provided through push buttons. LoRa RA-02 modules are connected to both ESP32 boards using SPI communication, enabling long-range wireless data transmission between the wearable unit and the monitoring receiver.

#### Build Photos


![Components](Images/HW_COMP.png)

![Build1](Images/Reciever.jpeg)
Displays the receiver-side ESP32 connected to the RA-02 LoRa module and antenna, responsible for capturing transmitted emergency packets and forwarding clean serial data to the Python-based monitoring system.

![Build1](Images/Transmitter.jpeg)
Shows the complete transmitter-side hardware setup on a breadboard, integrating the ESP32 microcontroller with the NEO-6M GPS module, RA-02 LoRa transceiver with external antenna, OLED display for status output, and interconnecting jumper wires for real-time emergency data transmission.

![Final](Images/Comb1.jpeg)
Shows the complete Rakshak 1.0 women safety system operating in real time, with the transmitter unit capturing GPS data and emergency events, the receiver unit receiving LoRa alerts, and the Python monitoring application simultaneously displaying processed emergency data and live location on OpenStreetMap along with instant desktop notifications.


### For Hardware Projects:

#### Bill of Materials (BOM)

| Component               | Quantity | Specifications                       | Price (Approx) | Link/Source |
| ----------------------- | -------- | ------------------------------------ | -------------- | ----------- |
| ESP32 Development Board | 2        | Dual-core 32-bit MCU, WiFi/Bluetooth | ₹600 each      | [Link]      |
| RA-02 LoRa Module       | 2        | 433 MHz long-range transceiver       | ₹450 each      | [Link]      |
| NEO-6M GPS Module       | 1        | UART GPS receiver with antenna       | ₹750           | [Link]      |
| OLED Display            | 1        | 0.96”, 128×64, I2C                   | ₹250           | [Link]      |
| ADXL335 Accelerometer   | 1        | 3-axis analog sensor                 | ₹300           | [Link]      |
| Breadboard              | 2        | 830 tie-points                       | ₹120 each      | [Link]      |
| Jumper Wires/Single strand wires          | 1 set    | Male-to-male                         | ₹100           | [Link]      |
| External LoRa Antenna   | 2        | SMA connector                        | ₹150 each      | [Link]      |
| USB Cable               | 2        | Micro USB                            | ₹80 each       | [Link]      |


Total Estimated Cost: ₹3,800 – ₹4,200 (approx.)


## Project Demo

### Video
https://drive.google.com/file/d/1A8QwXHA24_kf9ueLwoiRuupRxyYLmEjs/view?usp=drivesdk

This video demonstrates the basic working of the Rakshak 1.0 women safety system using the panic button mechanism. When the user presses the panic button, the ESP32 captures the emergency event, retrieves real-time GPS coordinates, and transmits the alert through the LoRa communication module. The receiver unit forwards the data to the Python monitoring system, which identifies the user through device ID mapping, displays the emergency location on a live map, and triggers an instant desktop notification for rapid response.

Key Highlights:
• Instant panic-triggered emergency transmission
• Real-time GPS location visualization
• User identification using device ID
• Live alert notification system


### Additional Demos
https://drive.google.com/file/d/1H0Yil8FEZiCKtIFs6si4XcBkV7o9oqfa/view?usp=drivesdk
This video demonstrates the advanced safety features of Rakshak 1.0, focusing on accelerometer-based fall detection and intelligent event code handling. The ADXL335 sensor continuously monitors motion patterns to detect free fall and impact conditions. Upon detecting a fall, the system generates a dedicated event code and transmits it via LoRa along with GPS data. The monitoring software interprets the event type, identifies the user, processes location information, and generates an emergency alert accordingly.

Key Highlights:
• Automatic fall detection using motion sensing
• Event-based emergency classification (panic vs fall)
• Reliable long-range LoRa communication
• Smart alert processing and monitoring

---

## AI Tools Used (Optional - For Transparency Bonus)

If you used AI tools during development, document them here for transparency:

**Tool Used:** ChatGPT

**Purpose:**
1. Debugging ESP32 and Python serial communication issues
2. Improving code structure and error handling
3. Generating documentation content (README, technical descriptions, BOM formatting)
4. Assisting in reverse geocoding integration logic

**Key Prompts Used:**
1. “Fix serial decoding errors in Python for ESP32 communication”
2. “Modify LoRa receiver code to output clean serial data”
3. “Explain GPS fallback handling in monitoring software”
4. “Format Bill of Materials for hardware project README”

**Percentage of AI-generated code:** Approximately 35–40%

**Human Contributions:**
- Architecture design and planning
- Custom business logic implementation
- Integration and testing

*Note: Proper documentation of AI usage demonstrates transparency and earns bonus points in evaluation!*

Made with ❤️ at TinkerHub
