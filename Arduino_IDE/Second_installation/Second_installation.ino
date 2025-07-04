#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_ST7789.h>
#include <SD.h>

// Pin Definitions
#define TFT_CS        7
#define TFT_DC        39
#define TFT_RST       40
#define TFT_backlight 45
#define I2C_SDA       42
#define I2C_SCL       41
#define BMP_Addr      0x77

// SD Card Pins (SPI)
#define SD_MISO  37
#define SD_MOSI  35
#define SD_SCK   36
#define SD_CS    10  // Chip Select (can be any GPIO)

// Constants
const float SEA_LEVEL_PRESSURE_HPA = 1013.25;  // Standard sea level pressure
const int PRESSURE_READINGS = 5;               // Number of samples for averaging
const int ALTITUDE_UPDATE_MS = 1000;           // Update interval

// Global Objects
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BMP280 bmp;

// Altimeter Variables
float referencePressure;  // Pressure at initial position (0m)
bool altimeterCalibrated = false;

// File name and content
//const char* filename = "/esp32_data.txt";
const char* dataToWrite = "ESP32-S3 SD Card Test - Hello World!\n"; 

File myFile;
String fileName = "test.txt";

// Create a dedicated SPI bus for SD card
SPIClass sdSPI(HSPI);

void initDisplay() {
  pinMode(TFT_backlight, OUTPUT);
  digitalWrite(TFT_backlight, HIGH);
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void initBMP() {
  if (!bmp.begin(BMP_Addr)) {
    tft.setCursor(0, 20);
    tft.print("BMP280 Error");
    Serial.println("BMP280 initialization failed!");
    while(1);
  }
  
  // Configure for high accuracy
  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );
}

float readPressure() {
  float sum = 0;
  for (int i = 0; i < PRESSURE_READINGS; i++) {
    sum += bmp.readPressure();
    delay(10);
  }
  return (sum / PRESSURE_READINGS) / 100.0;  // Convert to hPa
}

void calibrateAltimeter() {
  referencePressure = readPressure();
  altimeterCalibrated = true;
  Serial.print("Calibrated reference pressure: ");
  Serial.print(referencePressure);
  Serial.println(" hPa");
}

float calculateAltitude() {
  if (!altimeterCalibrated) return 0.0;
  
  float currentPressure = readPressure();
  // International barometric formula
  return 44330.0 * (1.0 - pow(currentPressure / referencePressure, 0.1903));
}

void displayAltitude(float altitude) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  
  tft.print("Altitude:");
  tft.setCursor(0, 30);
  tft.print(altitude, 2);  // Display with 2 decimal places
  tft.print(" m");
  
  Serial.print("Altitude: ");
  Serial.print(altitude, 2);
  Serial.println(" m");

}

// Write a string to the file
void writeData(String message) {
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    myFile.println(message);
    myFile.close();
    Serial.println("Data written: " + message);
  } else {
    Serial.println("Error writing to file");
  }
}

// Read and print file contents
void readMyFile() {
  myFile = SD.open(fileName);
  if (myFile) {
    Serial.println("File content:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Error opening file");
  }
}
 
/*
void writeToFile(const char* path, const char* message) {
  Serial.printf("Writing to file: %s\n", path);
  
  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  if (file.println(message)) {
    Serial.println("File written successfully");
  } else {
    Serial.println("Write failed");
  }
  
  file.close(); // Always close files!
}
*/

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.print("beginning initialization of SD card: ");

 // Initialize custom SPI bus for SD card
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  Serial.print("Initializing SD card... ");
  if (!SD.begin(SD_CS, sdSPI)) {
    Serial.println("FAILED. Check:");
    Serial.println("1. Wiring (MISO, MOSI, SCK, CS)");
    Serial.println("2. SD card format (must be FAT32)");
    Serial.println("3. Power (3.3V, not 5V)");
    while (1); // Halt
  }

  if(SD.exists(fileName))
  {
    SD.remove(fileName);
    Serial.println("\nFile already exists. It has been deleted and will be overwritten\n");
  }

  // Write Test Data
  writeData("Hello, SD Card!");
  
  // Read Back Data
  readMyFile();

  initDisplay();
  initBMP();
  
  // Calibrate altimeter (set current position as 0m)
  calibrateAltimeter();
  
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.print("Altimeter Ready");
  delay(1000);
}

void loop() {
  float new_pressure = bmp.readPressure() / 100;
  Serial.println("Pressure:");
  Serial.println(new_pressure);
  float altitude = calculateAltitude();

/*
  // Create a data string with timestamp
  String dataString = String(millis()) + "," + 
                     String(altitude, 2) + "," + 
                     String(new_pressure, 2);

  // Log to SD card
  writeToFile(filename, dataString.c_str());
*/

  displayAltitude(altitude);
  delay(ALTITUDE_UPDATE_MS);
}