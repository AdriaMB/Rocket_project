#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h> // RGB_LED
#include <Adafruit_GFX.h> // Display
#include <Adafruit_ST7789.h> // Display
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
String fileName = "data.txt";

unsigned long startTime;

/*
* ---------------------------------------------------------------------------------------
* Function Prototypes
* ---------------------------------------------------------------------------------------
*/
void readIMU();
void initBMP();
void initQMI();
float getAltitude();
void calibrateAltimeter();
/*
* ---------------------------------------------------------------------------------------
* Setup Function — runs once at startup
* @ brief Example Setup Code. Add and adjust Code as needed:
* ---------------------------------------------------------------------------------------
*/

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

  // Initialize QMI8658C
  initQMI();
  
  Serial.print("Initializing SD card... ");
  if (!SD.begin(SD_CS)) {
    Serial.println("FAILED. Check:");
    Serial.println("1. Wiring (MISO, MOSI, SCK, CS)");
    Serial.println("2. SD card format (must be FAT32)");
    Serial.println("3. Power (3.3V, not 5V)");
    while (1); // Halt
  }
  Serial.println("OK");

  // Delete existing file (optional)
  if (SD.exists(fileName)) {
    SD.remove(fileName);
    Serial.println("Old file deleted");
  }

  myFile = SD.open(fileName, FILE_WRITE);

  startTime = millis(); // Record start time
}

void loop() {

  //write to file code
  if(millis() - startTime < 2000)
  {
    float currentAltitude = getAltitude();
    if (myFile) 
    {
      // Write sample data (millis and random value)
      myFile.print(millis());
      myFile.print(", Altitude: ");
      myFile.println(currentAltitude);
      myFile.print("\n");

      //debug code     
      Serial.println("Data point written");
      delay(100); // Slow down writing to create multiple entries
    } 
    else 
    {
      Serial.println("Error writing to file");
    }

  }

  else
  {
    myFile.close(); //save written content
    while(1);
  }

}

// Define a conversion function
void readIMU() {
  int16_t raw[3];
  
  qmi.getAccelRaw(raw);
  acc.x = raw[0] * 0.061f;  // Scaling to g-forces
  acc.y = raw[1] * 0.061f;
  acc.z = raw[2] * 0.061f;
  
  qmi.getGyroRaw(raw);
  gyr.x = raw[0] * 0.0076f;  // Scaling to dps
  gyr.y = raw[1] * 0.0076f;
  gyr.z = raw[2] * 0.0076f;
}


/**
* @brief Initializes the BMP280 sensor and handles failure if not found.
*
* @note Uses I2C address defined by BMP_ADDR.
*/
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
/**
* @brief Initializes the QMI8658C IMU sensor and configures accelerometer and gyroscope.
*
* @note Uses I2C address defined by QMI_ADDR.
*/
void initQMI(){
  // Initialize QMI8658C Sensor (address is 0x6B)
  if (!qmi.begin(Wire,QMI_Addr,I2C_SDA,I2C_SCL)) {
    Serial.println("QMI not found");
    while (1);
  }
  // Configure QMI sensor
  qmi.enableGyroscope();
  qmi.enableAccelerometer();
  qmi.configAccelerometer(
  SensorQMI8658::ACC_RANGE_4G,
  SensorQMI8658::ACC_ODR_1000Hz,
  SensorQMI8658::LPF_MODE_0);
  qmi.configGyroscope(
  SensorQMI8658::GYR_RANGE_64DPS,
  SensorQMI8658::GYR_ODR_896_8Hz,
  SensorQMI8658::LPF_MODE_3);
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
