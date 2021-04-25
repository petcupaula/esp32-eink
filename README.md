# Arduino code for using an eInk display

## Components

* ESP32-integrated 2.13”e-ink display with built-in WiFi and Bluetooth (https://wiki.dfrobot.com/e-Ink_Display_Module_for_ESP32_SKU_DFR0676)

## Optional components for the code in the repo

### Air Quality Sensor

* CCS811 Air Quality Sensor - measure the concentration of carbon dioxide and TVOC(TotalVolatileOrganicCompounds) (https://wiki.dfrobot.com/CCS811_Air_Quality_Sensor_SKU_SEN0339)
* Pins: https://dfimg.dfrobot.com/nobody/wiki/2fa1421740d13800802149746e923579
  * SDA: 21, SCL: 22, Operating Voltage: 3.3V~5.5V, so we can connect to 3.3 on ESP32

From the sensor documentation (https://github.com/DFRobot/DFRobot_CCS811/blob/master/README.md):

Carbon dioxide concentration(ppm)  | Impact on human-beings
------------------ | ---------
more than 5000        |      toxic 
2500-5000        |      unhealthy to human body
1000-2500        |      feel sleepy 
500-1000        |      turbid air
less than 500        |      no effect

TVOC concentration (ppb)     | Impact on human-beings
------------------ | ---------
less than 50        |      no effect 
50-750        |      uncomfortable, nervous 
760-6000        |      uncomfortable, nervous, headache
more than 6000       |      headache, neurological problems 
