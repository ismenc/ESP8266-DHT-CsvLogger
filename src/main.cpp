/**
 * @file main.cpp
 * @author Ismael Nunez
 * @brief Minimal application to log readings from a DHT sensor to a SD card. 
 *        Provides some feedback with the built in led. 
 *        Built on PlatformIO & ESP8266 (Compatible with arduino framework). 
 *        Check out README.md
 *        Dependencies:
 * adafruit/SdFat - Adafruit Fork
 * adafruit/DHT sensor library
 * adafruit/Adafruit Unified Sensor
 * 
 * @version 1
 * @date 2022-01-20
 * 
 */

#include <SPI.h>
#include "DHT.h"
#include "SdFat.h"
#include <ctime>

// Stuff for debug environment (will enable or disable Serial usage
// depending on BUILD_TYPE set on the diferent platformio.ini envs)
#define SERIAL_PRINTER Serial
#define ENV_DEBUG 1
#ifndef BUILD_TYPE
  #define BUILD_TYPE 0
#endif
#if BUILD_TYPE==ENV_DEBUG
  #define DEBUG_PRINTF(...) \
    { SERIAL_PRINTER.printf(__VA_ARGS__); }
  #define DELAY_MULTIPLIER 100
#else
  #define DEBUG_PRINTF(...) {}
  #define DELAY_MULTIPLIER 1000
#endif

// Configure this
#define DHTPIN D2
#define DHTTYPE DHT11 
#define SD_CS_PIN SS
// Delays in seconds. Notice there's a delay multiplier which makes the program 10 times faster in debug
#define START_DELAY_SECONDS 20
#define SD_MOUNT_RETRY_SECONDS 30
#define DELAY_BETWEEN_READINGS_SECONDS 90

// We're using the compilation's build timestamp injected
// as program's start time in order to simplify.
// if you start the board later on, timestamp will be back in time
int buildTimeWithOffsets = BUILD_TIME + 30 + 3600;
time_t currentTime = buildTimeWithOffsets;
SdFat SD;
File dhtCsvFile;
DHT dht(DHTPIN, DHTTYPE);
uint8_t errorCount = 0;
const char* logFileNameStarter = "dhtLog_";

// Function definitions
uint8_t logDhtReadings (char* filename, float temp, float hum, float heatIndexTemp);
void initializeSerialIfProceed();
void initializeSdCardLoop();
void displayLedVisualError();
void displayLedVisualSuccess();
void formatTimeForCsv(char*);
void formatTimeForFileName(char*);

/* --------------------- Application --------------------- */

void setup() {

  initializeSerialIfProceed();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  SERIAL_PRINTER.printf("\nInitializing in 20 secs. Please insert SD card.\n");
  delay(START_DELAY_SECONDS * DELAY_MULTIPLIER);
  SERIAL_PRINTER.printf("Initializing SD card... ");

  initializeSdCardLoop();
}

void loop() {

  if(errorCount > 11) {
    initializeSdCardLoop();
  }

  char dhtLogFileName[15];
  formatTimeForFileName(dhtLogFileName);

  float rhum = dht.readHumidity();
  float temp = dht.readTemperature();
  float hic = dht.computeHeatIndex(temp, rhum, false);

  if(logDhtReadings(dhtLogFileName, temp, rhum, hic) != 0) {
    errorCount++;
  }

  delay(DELAY_BETWEEN_READINGS_SECONDS * DELAY_MULTIPLIER);
}

/* --------------------- Functions --------------------- */

/**
 * @brief If we run debug configuration serial communication 
 *        will be initialized and running
 */
void initializeSerialIfProceed() {
  #if BUILD_TYPE==ENV_DEBUG
    SERIAL_PRINTER.begin(9600);
    while (!SERIAL_PRINTER) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  #endif
}

/**
 * @brief Writes data in csv format to the SD card
 * 
 * @param filename file to write (will append at the end)
 * @param temp temperature
 * @param hum relative humidity
 * @param heatIndexTemp heat index
 * @return uint8_t error code, 0 for success
 */
uint8_t logDhtReadings (char* filename, float temp, float hum, float heatIndexTemp) {

  uint8_t errorCode = 1;
  SERIAL_PRINTER.printf("Opening %s\n", filename);
  dhtCsvFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (dhtCsvFile) {
    digitalWrite(LED_BUILTIN, LOW);
    SERIAL_PRINTER.printf("Writing to %s... ", filename);

    char timeString[20];
    formatTimeForCsv(timeString);
    dhtCsvFile.printf("%s,%.2f,%.0f,%.2f\r\n", timeString, temp, hum, heatIndexTemp);
    dhtCsvFile.close();

    SERIAL_PRINTER.printf("Reading saved at %s\n", timeString);
    errorCode = 0; errorCount = 0;
  } else {
    // if the file didn't open, print an error:
    SERIAL_PRINTER.printf("Error opening %s\n", filename);
    displayLedVisualError();
  }
  digitalWrite(LED_BUILTIN, HIGH);

  return errorCode;
}

/**
 * @brief This return timeChar in this format: 20/01/2022 09:12:06
 */
void formatTimeForCsv(char* timeChar) {
  // update time
  currentTime = buildTimeWithOffsets + (millis()/1000);
  // We do this way to add preceeding 0's on numbers
  tm* aux = gmtime(&currentTime);
  // This will write in this format: 20/01/2052 09:12:06
  sprintf(timeChar,"%s%d/%s%d/%d %s%d:%s%d:%s%d",
    aux->tm_mday < 10 ? "0" : "", aux->tm_mday, // day
    aux->tm_mon+1 < 10 ? "0" : "", aux->tm_mon+1, // month with
    aux->tm_year+1900, // Year
    aux->tm_hour < 10 ? "0" : "", aux->tm_hour, // hours
    aux->tm_min < 10 ? "0" : "", aux->tm_min, // mins
    aux->tm_sec < 10 ? "0" : "", aux->tm_sec // secs
  );
}

/**
 * @brief This return timeChar in this format: dhtLog_2022-01-20
 *        This will make logs rotate every next day
 */
void formatTimeForFileName(char* timeChar) {
  // This line updates the time variable (as it's not continuosly running)
  currentTime = buildTimeWithOffsets + (millis()/1000);
  tm* aux = gmtime(&currentTime);
  sprintf(timeChar,"%s%d-%s%d-%s%d",
    logFileNameStarter,
    aux->tm_year+1900, // Year
    aux->tm_mon+1 < 10 ? "0" : "", aux->tm_mon+1, // month with
    aux->tm_mday < 10 ? "0" : "", aux->tm_mday // day
  );

  // Uncomment this if you want logs to rotate by hour
  /*
  sprintf(timeChar,"%s%d-%s%d-%s%d_%s%d",
    logFileNameStarter,
    aux->tm_year+1900, // Year
    aux->tm_mon+1 < 10 ? "0" : "", aux->tm_mon+1, // month with
    aux->tm_mday < 10 ? "0" : "", aux->tm_mday, // day
    aux->tm_hour < 10 ? "0" : "", aux->tm_hour // hour
  );
  */
}

void initializeSdCardLoop() {

  digitalWrite(LED_BUILTIN, LOW);
  while (!SD.begin(SD_CS_PIN)) { // Here attempts to mount the SD
    // This lines down here are to improve user experience actually
    digitalWrite(LED_BUILTIN, HIGH);
    SERIAL_PRINTER.printf("SD initialization failed.\n");
    displayLedVisualError();
    delay(SD_MOUNT_RETRY_SECONDS * DELAY_MULTIPLIER);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  SERIAL_PRINTER.printf("SD initialization done.\n");
  displayLedVisualSuccess();
  errorCount = 0;
}

void displayLedVisualError() {
  
    digitalWrite(LED_BUILTIN, LOW);
    delay(120);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(120);
    digitalWrite(LED_BUILTIN, LOW);
    delay(120);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(350);
    digitalWrite(LED_BUILTIN, LOW);
    delay(120);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(120);
    digitalWrite(LED_BUILTIN, LOW);
    delay(120);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
}

void displayLedVisualSuccess() {
  
    digitalWrite(LED_BUILTIN, LOW);
    delay(220);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(220);
    digitalWrite(LED_BUILTIN, HIGH);
}