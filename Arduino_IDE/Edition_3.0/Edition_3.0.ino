
/*Our libraries and macros that are required for the project code to run.*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h> // RGB_LED
#include <Adafruit_BMP280.h> // BMP
#include <SensorQMI8658.hpp> // QMI

// SD Card Pins (Custom SPI Bus)
#define SD_MISO 37
#define SD_MOSI 35
#define SD_SCK  36
#define SD_CS   10  // Chip Select

// I2C Pins for Sensors
#define I2C_SDA 42
#define I2C_SCL 41
// Sensor Address
#define BMP_Addr 0x77
#define QMI_Addr 0x6B

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*Global variables used for the code*/
// BMP Sensor (Temperature and Pressure Sensor)
Adafruit_BMP280 bmp;
// QMI Sensor (6-axis Gyroscope & Accelerometer)
SensorQMI8658 qmi;
// Data structures for QMI Sensor readings
IMUdata acc;
IMUdata gyr;

//temperature variable
float temperature = 0;

// Global variables
float seaLevel_hPa;      // Will be calibrated at startup
float initialAltitude;   // Used to zero the altimeter
bool isCalibrated = false;

// File Handling
File myFile;
String fileName = "/test2.txt";

unsigned long startTime;
unsigned long lastLogTime = 0;
const unsigned long log_interval = 100; // Log every 100ms
const unsigned long flight_duration = 10000;
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

//functions
void initBMP();
float getAltitude();
void calibrateAltimeter();
void initSD();
void file_check();

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
/*Main code*/
void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for Serial

  // Initialize SD Card with Dedicated SPI Bus
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  // I2C initialization
  Wire.begin(I2C_SDA,I2C_SCL);
  // Initialize BMP280
  initBMP();
  //initialize altitude
  calibrateAltimeter();
  //initialize the sd card
  InitSD();
  //check if file already exists and delete old file, create new file with headers
  check_file();

  startTime = millis(); // Record start time

  delay(100);

}

void loop() {
  unsigned long currentTime = millis();
  delay(1000);
  
  if(currentTime - lastLogTime >= log_interval)
  {
    if(currentTime - startTime < flight_duration)
    {
      myFile = SD.open(fileName, FILE_APPEND);
      // Read sensors
      float currentAltitude = getAltitude();
      float currentTemperature = bmp.readTemperature();
      float currentPressure = bmp.readPressure() / 100.0; // Convert to hPa

      if (myFile) 
      {
        // Write data as CSV
        myFile.print(currentTime);
        myFile.print(",");
        myFile.print(currentAltitude, 2);
        myFile.print(",");
        myFile.print(currentTemperature, 2);
        myFile.print(",");
        myFile.println(currentPressure, 2);
        myFile.close();

        Serial.println("good shit");
        lastLogTime = currentTime;
      } 
      else 
      {
        Serial.println("Error opening file for writing!");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/
//initialize the BMP barometric pressure sensor
void initBMP(){
  // Initialize BMP280 Sensor (address is 0x77)
  if (!bmp.begin(BMP_Addr)) {
    Serial.println("BMP not found");
    while (1);
  }
  /* Default settings from datasheet.  */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500);  /* Standby time. */
}

// Returns altitude relative to starting point (0m initially)
float getAltitude() {
  float pressure = bmp.readPressure() / 100.0; // Convert to hPa
  float altitude = 44330.0 * (1.0 - pow(pressure / seaLevel_hPa, 0.1903));
  return altitude - initialAltitude; // Normalize to 0m at startup
}

// Calibrate sea-level pressure and set initial altitude to 0m
void calibrateAltimeter() {
  // Take 10 readings to stabilize (optional but recommended)
  float sumPressure = 0;
  for (int i = 0; i < 10; i++) {
    sumPressure += bmp.readPressure() / 100.0;
    delay(100);
  }
  seaLevel_hPa = sumPressure / 10.0;

  // Calculate initial altitude (will subtract later to zero it)
  initialAltitude = 44330.0 * (1.0 - pow(seaLevel_hPa / seaLevel_hPa, 0.1903)); // Should be ~0m
  Serial.print("Calibrated seaLevel_hPa: "); Serial.println(seaLevel_hPa);
}

//initialize the SD card 
void InitSD()
{
  Serial.print("Initializing SD card... ");
  if (!SD.begin(SD_CS)) {
    Serial.println("FAILED. Check:");
    Serial.println("1. Wiring (MISO, MOSI, SCK, CS)");
    Serial.println("2. SD card format (must be FAT32)");
    Serial.println("3. Power (3.3V, not 5V)");
    while (1); // Halt
  }
  Serial.println("OK");
}

//check_file function
void check_file()
{
  // Remove old file if it exists
  if (SD.exists(fileName)) 
  {
    SD.remove(fileName);
    Serial.println("Old data file removed");
  }

  // Create new file and write header
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    myFile.println("Time(ms),Altitude(m),Temperature(C),Pressure(hPa)");
    myFile.close();
  } else {
    Serial.println("Failed to create log file.");
    while (true);
  }
}