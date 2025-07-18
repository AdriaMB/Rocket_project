
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
String fileName = "/project.txt";

unsigned long startTime;
unsigned long previousTime;
unsigned long lastLogTime = 0;
const unsigned long log_interval = 500; // Log every 100ms
const unsigned long flight_duration = 100000;

// Global calibration variables
float verticalOffset = 0;
bool isCalibrated2 = false;
const int CALIBRATION_SAMPLES = 50;
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

//functions
void initBMP();
float getAltitude();
void calibrateAltimeter();
void initSD();
void file_check();
void write(unsigned long currentTime);
float getAmbientTemperature();
void initQMI();
void readIMU();
float getVerticalAcceleration();
void calibrateVerticalAcceleration();
float getVerticalVelocity();

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
  // Initialize the QMI sensor
  initQMI();
  //initialize altitude
  calibrateAltimeter();
  //initialize the sd card
  InitSD();
  //check if file already exists and delete old file, create new file with headers
  check_file();

  startTime = millis(); // Record start time
  previousTime = millis();

  delay(100);

}

void loop() {
  unsigned long currentTime = millis();
  float acceleration = 0.0;
  float velocity = 0.0;
  delay(1000);
  
  //log data every 100ms 
  if(currentTime - lastLogTime >= log_interval)
  {
    //check if the flight duration has been reached
    if(currentTime - startTime < flight_duration)
    {
      write(currentTime); //write data out to sd card
      acceleration = getVerticalAcceleration();
      delay(100);
      velocity = getVerticalVelocity();
      delay(100);
      Serial.println("acceleration: ");
      Serial.println(acceleration);
      Serial.println("Velocity: ");
      Serial.println(velocity);
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
    myFile.println("Time(ms),Altitude(m),Temperature(C),Pressure(hPa),Acceleration(ms/2),Velocity(ms/-1),Accelerometer_Coordinates,Gyroscope_Coordinates");
    myFile.close();
  } else {
    Serial.println("Failed to create log file.");
    while (true);
  }
}

//this function writes the data out to the sd card 
void write(unsigned long currentTime)
{
  myFile = SD.open(fileName, FILE_APPEND);
      // Read sensors
      float currentAltitude = getAltitude();
      float currentTemperature = getAmbientTemperature();
      float currentPressure = bmp.readPressure() / 100.0; // Convert to hPa
      float accel = getVerticalAcceleration();
      float vel = getVerticalVelocity();

      if (myFile) 
      {
        // Write data as CSV
        myFile.print(currentTime);
        myFile.print(",");
        myFile.print(currentAltitude, 2);
        myFile.print(",");
        myFile.print(currentTemperature, 2);
        myFile.print(",");
        myFile.print(currentPressure, 2);
        myFile.print(",");
        myFile.print(accel, 2);
        myFile.print(",");
        myFile.print(vel,2);
        myFile.print(",");
        myFile.print(acc.x);myFile.print("/");myFile.print(acc.y);myFile.print("/");myFile.print(acc.z);
        myFile.print(",");
        myFile.print(gyr.x);myFile.print("/");myFile.print(gyr.y);myFile.print("/");myFile.println(gyr.z);
        myFile.close();

        Serial.println("Altitude: "); Serial.println(currentAltitude);
        Serial.println("Temperature: "); Serial.println(currentTemperature);
        lastLogTime = currentTime;
      } 
      else 
      {
        Serial.println("Error opening file for writing!");
      }
}

//gets proper temperature reading by adjusting the chips internal temperature
float getAmbientTemperature() {
  float chipTemp = bmp.readTemperature();
  float vcc = 3.3; // ESP32 supply voltage
  // Empirical correction (adjust coefficients based on testing)
  return chipTemp - (0.5 * vcc) - 1.2;
}

//set up the qmi sensor to enable the accelerometer and gyroscope
void initQMI(){
  // Initialize QMI8658C Sensor (address is 0x6B)
  if (!qmi.begin(Wire,QMI_Addr,I2C_SDA,I2C_SCL)) { 
    // if Sensor could not be found print error
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

/*
void readIMU() {
  qmi.getAccelerometer(acc.x, acc.y, acc.z); // In g's (9.81 m/s²)
  qmi.getGyroscope(gyr.x, gyr.y, gyr.z);     // In degrees per second
  
  Serial.printf("Acc: X=%.2fg Y=%.2fg Z=%.2fg\n", acc.x, acc.y, acc.z);
  Serial.printf("Gyro: X=%.2f°/s Y=%.2f°/s Z=%.2f°/s\n", gyr.x, gyr.y, gyr.z);
}
*/

float getVerticalAcceleration() {
  if (!isCalibrated) {
    calibrateVerticalAcceleration();
  }
  
  qmi.getAccelerometer(acc.x, acc.y, acc.z);
  qmi.getGyroscope(gyr.x, gyr.y, gyr.z);
  
  // Convert from g to m/s² and remove gravity offset
  // Use only the vertical axis (Y in this example)
  float verticalAccel = (acc.y - verticalOffset) * 9.80665;
  
  // Optional: Apply simple low-pass filter to reduce noise
  static float filteredAccel = 0;
  filteredAccel = 0.9 * filteredAccel + 0.1 * verticalAccel;
  
  return filteredAccel;
}

float getVerticalVelocity() {
  static float velocity = 0;
  static unsigned long lastTime = millis();
  
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0f;
  lastTime = currentTime;
  
  float acceleration = getVerticalAcceleration();
  velocity += acceleration * dt;
  
  // Small decay to prevent drift (adjust factor as needed)
  velocity *= 0.995;
  
  return velocity;
}

//calibrating the accelerometer
void calibrateVerticalAcceleration() {
  float sum = 0;
  
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    qmi.getAccelerometer(acc.x, acc.y, acc.z);
    qmi.getGyroscope(gyr.x, gyr.y, gyr.z);
    sum += acc.y;  // Assuming Y is your vertical axis
    delay(10);
  }
  
  verticalOffset = sum / CALIBRATION_SAMPLES;
  isCalibrated2 = true;
  Serial.print("Calibration complete. Vertical offset: ");
  Serial.println(verticalOffset);
}
