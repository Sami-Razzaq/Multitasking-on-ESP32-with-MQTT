# Multitasking-on-ESP32-with-MQTT-Broker
Interfacing multiple sensors on ESP32, measuring their values, and sending data to the MQTT-based NodeRed server.

Utilizing the multitasking and wireless connection capabilities of ESP32, this project reads data from multiple sensors pinned to one core. It sends them over an MQTT Broker to NodeRed server pinned on the second core simultaneously. 

The MQTT Tasks of maintaining connection and sending data are done on Core0.
The DHT11, ACS712, and Ultrasonic Sensor values are read using Core1.
A dashboard made on the NodeRed server over the Mosquitto Broker displays all results.

Components:
1. ESP32
2. DHT11
3. ACS712
4. Ultrasonic Sensor
5. Mosquitto Broker
6. NodeRed Server
