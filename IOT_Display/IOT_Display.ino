// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>

#include "icon.h"

#define BRIGHTNESS 0
#define TEMP       1
#define HUMIDITY   2
#define HEAT       3

#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional) from 0-255

#define DHT_PIN 2
#define DHT_TYPE DHT11
// Use hardware SPI (faster - on Uno: 13-SCK, 12-MISO, 11-MOSI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);
// Use software SPI (slower)
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED, TFT_BRIGHTNESS);

// Initialize DHT sensor.
DHT dht(DHT_PIN, DHT_TYPE);

// Initialize LightSensor BH1750
BH1750 LightSensor(0x23);

// Variables and constants
uint16_t x, y;
boolean flag = false;

uint16_t preTime = 0;
uint16_t curTime = 0;
const uint16_t minDelay = 2000;

float humidity_index = 0;
float temperature_index = 0;
float brightness_index = 0;
float heat_index = 0;

float old_humidity_index = 0;
float old_temperature_index = 0;
float old_brightness_index = 0;
float old_heat_index = 0;

void warning_mess(float heat_index_ref);
void icon_display();

void setup() {
  // put your setup code here, to run once:
  tft.begin();
  dht.begin();
  Wire.begin();
  LightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  Serial.begin(9600);
  tft.clear();
  tft.setOrientation(1);
  tft.setBackgroundColor(COLOR_BLACK);
  tft.setFont(Terminal6x8);
  icon_display();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - preTime) > minDelay) {
    preTime = millis();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity_index = dht.readHumidity();

    // Read temperature as Celsius (the default)
    temperature_index = dht.readTemperature();


    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity_index) || isnan(temperature_index)) {
      //      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    heat_index = dht.computeHeatIndex(temperature_index, humidity_index, false);
    // Get Lux value from BH1750 Sensor
    brightness_index = LightSensor.readLightLevel();
    Serial.println(brightness_index);
    tft.setFont(Terminal6x8);
    //    tft.fillRectangle(30, 0, 69, 176, COLOR_BLACK);
    if (brightness_index != old_brightness_index) {
      tft.fillRectangle(30, 20, 80, 28, COLOR_BLACK);
      tft.drawText(30, 20, String(brightness_index));
      old_brightness_index = brightness_index;
    }
    if (temperature_index != old_temperature_index) {
      tft.fillRectangle(30, 54, 80, 62, COLOR_BLACK);
      tft.drawText(30, 54, String(temperature_index));
      old_temperature_index = temperature_index;
    }
    if (humidity_index != old_humidity_index) {
      tft.fillRectangle(30, 88, 80, 96, COLOR_BLACK);
      tft.drawText(30, 88, String(humidity_index));
      old_humidity_index = humidity_index;
    }
    if (heat_index != old_heat_index) {
      tft.fillRectangle(30, 122, 80, 130, COLOR_BLACK);
      tft.drawText(30, 122, String(heat_index));
      old_heat_index = heat_index;
    }
    //    tft.drawText(30, 20, String(brightness_index));
    //    tft.drawText(30, 54, String(temperature_index));
    //    tft.drawText(30, 88, String(humidity_index));
    //    tft.drawText(30, 122, String(heat_index));
    warning_mess(heat_index);
  }
}

void warning_mess(float heat_index_ref) {
  tft.setFont(Terminal12x16);
  if (heat_index_ref >= 27 && heat_index_ref <= 32) {
    //    tft.setBackgroundColor(0xFFF3);
    tft.drawText(30, 150, "Caution", 0xFFF3);
  }
  else if (heat_index_ref > 32 && heat_index_ref <= 41) {
    //    tft.setBackgroundColor(0xFE82);
    tft.drawText(30, 150, "Extreme Caution", 0xFE82);
  }
  else if (heat_index_ref > 41 && heat_index_ref <= 54) {
    //    tft.setBackgroundColor(0xFB20);
    tft.drawText(30, 150, "Danger", 0xFB20);
  }
  else if (heat_index_ref >= 54) {
    //    tft.setBackgroundColor(0xC800);
    tft.drawText(30, 150, "Extreme Danger", 0xC800);
  }
  else {
    //    tft.setBackgroundColor(COLOR_BLACK);
    tft.fillRectangle(0, 150, 220, 170, COLOR_BLACK);
  }
}

void icon_display() {
  tft.drawBitmap(0, 10, brightness_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 44, temperature_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 78, humidity_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 112, heat_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawText(81, 20, "lx");
  tft.drawText(81, 54, "*C");
  tft.drawText(81, 88, "%");
}


