# Minimal DHT sensor Csv logger

Simple. Read a **DHT sensor** and write log collected data to a **SD card**.

I'm sure that if you are newbie you can learn a lot on this project!
<br><br>
<div align="center">

![hardwareConfig](./img/hardwareConfig.jpg?raw=true "Hardware config")
![Csv content](./img/csvContent.jpg?raw=true "Csv content")
</div>

## Features
* Mount a SD card
* Read a DHT sensor (temperature + relative humidity + calculated heat index)
* Uses the time of compilation (time of writing to device) as start time for the program. A upgrade should be call the internet for getting time from an API or adding a hardware clock
* Writes to a log file which rotates every day
* Good practices such as **build configurations**, time and manipulation, documentation, etc
* Using the board built in led to give user feedback (when SD is being used)


It has been built on **PlatformIO** and ESP8266, it should compatible with any board using arduino.
