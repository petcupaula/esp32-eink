/*
 * Air quality monitoring on ESP32 eink display
 * 
 * Sensor library: https://github.com/DFRobot/DFRobot_CCS811
 */

#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"
#include <GxGDEH0213B72/GxGDEH0213B72.h>
#include "DFRobot_CCS811.h"
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18
#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17
#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2
#define BUTTON_PIN 39

GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);
SPIClass sdSPI(VSPI);
DFRobot_CCS811 CCS811;

bool sdOK = false;
typedef enum
{
    RIGHT_ALIGNMENT = 0,
    LEFT_ALIGNMENT,
    CENTER_ALIGNMENT,
} Text_alignment;
const uint8_t Whiteboard[1700] = {0x00};

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  display.init(); // enable diagnostic output on Serial

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0, 0);

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)){
    sdOK = false;
  } else {
    sdOK = true;
  }

  display.update();
  
  while(CCS811.begin() != 0){
      Serial.println("failed to init chip, please check if the chip connection is fine");
      delay(1000);
  }
  // set baseline based on readBaseline readings
  // will write a baseline number (hexadecimal) into register
  //CCS811.writeBaseLine(0x447B);
}
void loop() {
  if(CCS811.checkDataReady() == true){
      // get the current baseline number (hexadecimal)

      uint16_t baseline = CCS811.readBaseLine();
      uint16_t co2 = CCS811.getCO2PPM();
      uint16_t tvoc = CCS811.getTVOCPPB();
      
      Serial.print("Baseline: ");
      Serial.print(baseline, HEX);
      Serial.print(", CO2: ");
      Serial.print(co2);
      Serial.print("ppm, TVOC: ");
      Serial.print(tvoc);
      Serial.println("ppb"); 

      // Print baseline if you want to do some calibration
      displayText("Baseline:" + String(baseline, HEX), 30, CENTER_ALIGNMENT);
      // Else print some friendly text
      //displayText("AIR QUALITY", 30, CENTER_ALIGNMENT);
      displayText("CO2: " + String(co2) + " ppm", 60, CENTER_ALIGNMENT);
      displayText("TVOC: " + String(tvoc) + " ppb", 90, CENTER_ALIGNMENT);
      display.updateWindow(22, 0,  222,  120, true);
      display.fillScreen(GxEPD_WHITE);
  } else {
      Serial.println("Data is not ready!");
  }
  // Once we have a baseline, then add the value here
  //CCS811.writeBaseLine(0x447B);
  //delay cannot be less than measurement cycle
  delay(1000);
}

void displayText(const String &str, uint16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

  switch (alignment)
  {
  case RIGHT_ALIGNMENT:
    display.setCursor(display.width() - w - x1, y);
    break;
  case LEFT_ALIGNMENT:
    display.setCursor(0, y);
    break;
  case CENTER_ALIGNMENT:
    display.setCursor(display.width() / 2 - ((w + x1) / 2), y);
    break;
  default:
    break;
  }
  display.println(str);
}
