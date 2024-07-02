#include <Arduino.h>
#include <ACS712.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
#define PIN_SPI_CS 5
#define VOLTAGE 227

float power_factor = 0.365;

int sensorPin = 25;

File myFile;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ACS712 currentSensor(35, 3.3, 4095, 185);

unsigned long long currentTime = 0;
unsigned long long previousTime = 0;
unsigned long long delayTime = 30000; // write to sd card every 30 seconds

void setup()
{
  Serial.begin(115200);

  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  if (!SD.begin(PIN_SPI_CS))
  {
    while (1)
    {
      Serial.println(F("SDCARD Failed"));
      delay(1000);
    }
  }

  Serial.println("SD INITIALIZED");
  Serial.println("OLED INITIALIZED");
  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(2);
  display.setTextColor(WHITE);

  delay(5000);
}

void loop()
{
  currentTime = millis();
  display.clearDisplay();

  float current = currentSensor.mA_AC() / 1000;
  (current <= 0.1) ? (current = 0) : (current += 0.2);
  // float voltage = voltageSensor.getVoltageAC();

  float Wattage = current * VOLTAGE * power_factor;

  float Energy = ((Wattage / 1000.0) * ((currentTime / 1000.0) / 3600.0));

  display.setCursor(0, 10);
  display.printf("%.2f A\n", current);
  display.printf("%.1f W\n", Wattage);
  display.printf("%.4f KWh", Energy);

  Serial.printf("%.2f A\n", current);
  Serial.printf("%.2f,%.5f\n", Wattage, Energy);

  if ((currentTime - previousTime) > delayTime)
  {
    myFile = SD.open("/PowerReadingsPROPER2.txt", FILE_APPEND);
    myFile.printf("%.1f,%.5f\n", Wattage, Energy);
    myFile.close();
    previousTime = currentTime;
  }

  display.display();

  delay(1000);
}
