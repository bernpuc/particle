# particle
## Weather Station prototype
### Raspberry Pi Project

Controls a PMS5003 particle sensor and a SHT40 humidity and temperature sensor. The Pi runs a prometheus server which aggregates data and makes it available to Grafana dashboard. I wanted a way to see my sensor readings located at my house, from any location with internets. There is a more detailed README in the project folder `pi`.
#### Parts
1. Raspberry Pi 4
2. SHT40 sensor
3. PMS5003 sensor

### Arduino Project
Particle Sensor with Display is a simple project with an SSD1306 OLED display and the PMS5003 particle sensor. Initial version displays current particulate readings. Subsequent version will implement an AQI calculator. One issue is display real estate is cramped. If I could implement the AQI calculation, then I could remove the individual particle concentration values display and attach the SHT40 and have a nice Temperature, Humidity, and AQI display.
#### Parts
1. Arduino Uno
2. SSD1306 OLED (64x128)
3. PMS5003 
