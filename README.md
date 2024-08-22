# EspressoShotTimer
A simple reed switch activated espresso shot timer for machines using an AC powered brew valve (La Marzocco, Rancilio, Nuova Simoneli and others). 

My setup: 
ESP32-WROOM-DA Module from HiLetGo (purchased through Amazon)
HAMLIN - 59135-010 - MAGNETIC SENSOR (this is a high quality reed switch in a sealed package for high heat environments, I purchased through eBay)
10uF capacitor (connected to the GPIO for the reed/magnetic sensor and ground to counter act the frequency of AC current through the brew valve)
SSD1306 OLED Display -- I am using a .91" 128x32 display -- other displays can be used with small code adjustments(purchased through Amazon)

