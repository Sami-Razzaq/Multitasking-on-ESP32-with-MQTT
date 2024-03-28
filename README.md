# Multitasking-on-ESP32-with-MQTT
Interfacing multiple sensors on ESP32, measuring their values, and sending data to the MQTT-based NodeRed server.

Utilizing the multitasking and wireless connection capabilities of ESP32, this project reads data from multiple sensors pinned to one core. It sends them over an MQTT Broker to the NodeRed server pinned on the second core simultaneously. 

The MQTT Tasks of maintaining connection and sending data are done on Core0.
The DHT11, ACS712, and Ultrasonic Sensor values are read using Core1.
A dashboard made on the NodeRed server over the Mosquitto Broker displays all results.
The dashboard displays all sensors' data on gauge, graphs, and in tables.

**Components:**
1. ESP32
2. DHT11
3. ACS712
4. Ultrasonic Sensor
5. Mosquitto Broker
6. NodeRed Server

**Block Diagram**
![image](https://github.com/Sami-Razzaq/Multitasking-on-ESP32-with-MQTT-Broker/assets/67017724/704d7d4f-4265-4d43-9da3-4e172549d551)



**NodeRed Server Dashboard Setup**
![image](https://github.com/Sami-Razzaq/Multitasking-on-ESP32-with-MQTT-Broker/assets/67017724/dbf76859-b347-43ad-88bc-8013c8190bf4)



**Dashboard 1**
![image](https://github.com/Sami-Razzaq/Multitasking-on-ESP32-with-MQTT-Broker/assets/67017724/e734a7d7-73f0-4ce6-848e-6c0bab287631)



**Dashboard 2**
![image](https://github.com/Sami-Razzaq/Multitasking-on-ESP32-with-MQTT-Broker/assets/67017724/645851ae-ed4c-447a-8419-dfcdd95aefb6)
