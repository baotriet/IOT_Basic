// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "DHT.h"
#include <BH1750FVI.h>
#include <Wire.h>

#include "icon.h"



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
BH1750FVI LightSensor;

// Variables and constants
uint16_t x, y;
boolean flag = false;

uint16_t preTime = 0;
uint16_t curTime = 0;
uint16_t minDelay = 2000;

float humidity_index = 0;
float temperature_index = 0;
float brightness_index = 0;
float heat_index = 0;

void warning_mess(float heat_index_ref);
void icon_display();

void setup() {
  // put your setup code here, to run once:
  tft.begin();
  dht.begin();
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_L);
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  Serial.begin(9600);
  tft.clear();
  tft.setOrientation(1);
  tft.setBackgroundColor(COLOR_BLACK);
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
    brightness_index = LightSensor.GetLightIntensity();

    tft.setFont(Terminal6x8);
    tft.fillRectangle(30, 0, 69, 176, COLOR_BLACK);
    tft.drawText(30, 20, String(brightness_index));
    tft.drawText(70, 20, "Lux");
    tft.drawText(30, 54, String(temperature_index));
    tft.drawText(70, 54, "*C");
    tft.drawText(30, 88, String(humidity_index));
    tft.drawText(70, 88, "%");
    tft.drawText(30, 122, String(heat_index));
    warning_mess(heat_index);
  }
}

void warning_mess(float heat_index_ref) {
  tft.setFont(Terminal12x16);
  if (heat_index_ref >= 27 && heat_index_ref <= 32) {
    tft.setBackgroundColor(0xFFF3);
    tft.drawText(70, 150, "Caution", COLOR_BLACK);
  }
  else if (heat_index_ref > 32 && heat_index_ref <= 41) {
    tft.setBackgroundColor(0xFE82);
    tft.drawText(70, 150, "Extreme Caution", COLOR_BLACK);
  }
  else if (heat_index_ref > 41 && heat_index_ref <= 54) {
    tft.setBackgroundColor(0xFB20);
    tft.drawText(70, 150, "Danger", COLOR_BLACK);
  }
  else if (heat_index_ref >= 54) {
    tft.setBackgroundColor(0xC800);
    tft.drawText(70, 150, "Extreme Danger", COLOR_BLACK);
  }
  else {
    tft.setBackgroundColor(COLOR_BLACK);
    //    tft.drawText(30, 150, "", COLOR_BLACK);
  }
}

void icon_display() {
  tft.drawBitmap(0, 10, brightness_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 44, temperature_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 78, humidity_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
  tft.drawBitmap(0, 112, heat_icon, BMPWIDTH, BMPHEIGHT, COLOR_WHITE);
}

