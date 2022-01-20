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

## Want to test?

1. :open_file_folder: Clone or download.
2. :memo: Add project to PlatformIO (it's a VSCode or Atom extension).
3. :memo: Configure your board and platform on `platformio.ini` (all configurations, if neccesary create new project from ui and copy from it).
4. :memo: Configure constants on top of `main.cpp`
5. :ballot_box_with_check: Select the `env:debug` configuration.
6. :rocket: Hit `build`, `upload` and `monitor`.
7. :sparkles: Watch the serial output or checkout the sdcard after a while.

If you have any question about something specific, open an issue (but i won't be a teacher).