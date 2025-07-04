/*
* ---------------------------------------------------------------------------------------
* Project Name: Start Project
* Author: Dovydas Klisys
* Date: 15/06/25
* Version: 0.1
* Description: Simple start project to initialize all onboard modules:
* - TFT Display, BMP280, QMI8658C, RGB-LED
* - Select "ESP32S3 Dev Module" in the Arduino Board Manager.
* - Ensure all required libraries are installed.
*
* Hardware: ESP32-S3 FH4R2, 1.14" TFT Display, BMP280, QMI8658C, WS2812 RGB-LED
*
* Initial Setup: To enter boot mode and programm your ESP32S2:
* 1. Connect the ESP32S3 via USB.
* 2. Hold the "BOOT" button.
* 3. While holding "BOOT", press and release the "RST" button.
* 4. Release "BOOT".
*
* Warning: The RGB LED and QMI8658C sensor cannot be used simultaneously
* when connected to GPIO33, due to a conflict in the QMI library.
* ---------------------------------------------------------------------------------------
*/
/*
* ---------------------------------------------------------------------------------------
* Libraries
* ---------------------------------------------------------------------------------------
*/


// Add or remove as needed
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h> // RGB_LED
#include <Adafruit_GFX.h> // Display
#include <Adafruit_ST7789.h> // Display
#include <Adafruit_BMP280.h> // BMP
#include <SensorQMI8658.hpp> // QMI
/*
* ---------------------------------------------------------------------------------------
* Pin Definitions
* ---------------------------------------------------------------------------------------
*/
// RGB LED
// (GPIO33 conflicts with QMI8658C INT pin, do not use parallel)
#define LED_PIN 33
#define NUM_LEDS 1
// TFT Display Pins
#define TFT_CS 7
#define TFT_DC 39
#define TFT_RST 40
#define TFT_backlight 45
// SPI Pins
#define SPI_SCK 36
#define SPI_MISO 37
#define SPI_MOSI 35
// I2C Pins for Sensors
#define I2C_SDA 42
#define I2C_SCL 41
// Sensor Address
#define BMP_Addr 0x77
#define QMI_Addr 0x6B

// SD Card Pins (Custom SPI Bus)
#define SD_MISO 37
#define SD_MOSI 35
#define SD_SCK  36
#define SD_CS   10  // Chip Select

/*
* ---------------------------------------------------------------------------------------
* Global Objects/Varibles
* ---------------------------------------------------------------------------------------
*/
// TFT-Display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// RGB LED
Adafruit_NeoPixel RGB_LED = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
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

/*
* ---------------------------------------------------------------------------------------
* Function Prototypes
* ---------------------------------------------------------------------------------------
*/
void readIMU();
void initDisplay();
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
  // Start serial interface
  Serial.begin(115200);
  // write on serial interface
  Serial.println("Starting...");
  // Start SPI and I2C communication
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, TFT_CS);
  Wire.begin(I2C_SDA,I2C_SCL);
  // Initialize TFT Display
  initDisplay();
  // write on TFT Display
  // Cursor position (x,y)
  tft.setCursor(10, 10);
  tft.print("Starting...");
  // Initialize BMP280
  initBMP();

  //initialize altitude
  calibrateAltimeter();

  // Initialize QMI8658C
  initQMI();
  Serial.println("Setup finished");
  // Wait for 1000ms
  delay(1000);
}

/*
* ---------------------------------------------------------------------------------------
* Loop Function — runs repeatedly
* @brief Example Code. Put your own Code here:
* ---------------------------------------------------------------------------------------
*/
void loop() {
  // Refresh Display
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);

  float currentAltitude = getAltitude();
  Serial.print("Relative Altitude (m): "); 
  Serial.println(currentAltitude);
  delay(100);


  //commented out/ will be used later but not yet
  /*
  tft.print("Welcome:");

  tft.fillRect(10, 35, tft.width(), 20, ST77XX_BLACK); //just refresh area under welcome:

  //new section: collecting data for accelerometer and gyroscope then outputting out to serial port
  readIMU();
  tft.setCursor(10,35);
  tft.print("Accel (g): X="); tft.print(acc.x);
  tft.print("\nY="); tft.print(acc.y);
  tft.print(" Z="); tft.println(acc.z);

  tft.setCursor(10,70);
  tft.print("Gyro(dps): X="); tft.print(gyr.x);
  tft.print("\nY="); tft.print(gyr.y);
  tft.print(" Z="); tft.println(gyr.z);
  delay(5000);
  
  //visual update code
  tft.fillCircle(5, 40, 3, ST77XX_GREEN);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  tft.fillCircle(5, 40, 3, ST77XX_BLACK);
  digitalWrite(LED_PIN, LOW);

  //refresh screen to temporary show altimeter data
  tft.fillScreen(ST77XX_BLACK);
  getAltitude();
  tft.setCursor(10,10);
  tft.print("Altitude: ");
  tft.print(altitude);
  tft.print(" m");
  delay(5000);
  */

}


/*
* ---------------------------------------------------------------------------------------
* Functions
* ---------------------------------------------------------------------------------------
*/
/**
* @brief Initializes the TFT display, sets rotation, background color, text color and
size.
*/

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

void initDisplay(){
  // Turn on TFT backlight
  pinMode(TFT_backlight, OUTPUT);
  digitalWrite(TFT_backlight, HIGH);
  // Initialize TFT Display
  tft.init(135, 240); // Width x Height
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2); // Adjust as needed
}
/**
* @brief Initializes the BMP280 sensor and handles failure if not found.
*
* @note Uses I2C address defined by BMP_ADDR.
*/
void initBMP(){
  // Initialize BMP280 Sensor (address is 0x77)
  if (!bmp.begin(BMP_Addr)) {
    // if Sensor could not be found print error
    tft.setCursor(0, 20);
    tft.print("BMP not found");
    Serial.println("BMP not found");
    // endless loop
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
    // if Sensor could not be found print error
    tft.setCursor(0, 20);
    tft.print("QMI not found");
    Serial.println("QMI not found");
    // endless loop

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
/*
* ---------------------------------------------------------------------------------------
* End of file
* ---------------------------------------------------------------------------------------
*/