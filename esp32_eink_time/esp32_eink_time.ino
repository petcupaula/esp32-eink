#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"

#include <WiFi.h>
#include "time.h"

const char* ssid = ""; // wifi network name
const char* password = ""; // wifi password

const char* ntpServer = "ntp.ntsc.ac.cn"; // local ntp server
const uint32_t  gmtOffset_sec = 1*3600; // GMT+01:00, change depending on timezone
const uint16_t   daylightOffset_sec = 3600; // 0 if daylight saving time is off, 3600 otherwise

#include <GxGDEH0213B72/GxGDEH0213B72.h>
#include <Fonts/FreeMonoBold18pt7b.h>

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

typedef enum
{
    RIGHT_ALIGNMENT = 0,
    LEFT_ALIGNMENT,
    CENTER_ALIGNMENT,
} Text_alignment;


GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

SPIClass sdSPI(VSPI);

const uint8_t Whiteboard[1700] = {0x00};

uint16_t Year = 0 , Month = 0 , Day = 0 , Hour = 0 , Minute = 0 , Second = 0;
char Date[]={"01/01/2000"};
char Time[]={"00:00:00"};
bool sdOK = false;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  display.init(); // enable diagnostic output on Serial

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 0);

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)) {
    sdOK = false;
  } else {
    sdOK = true;
  }

  display.fillScreen(GxEPD_WHITE);
  display.update();
}

void loop()
{
  getTimeFromNTP("EU");
  displayText(String(Date), 60, CENTER_ALIGNMENT);
  displayText(String(Time), 90, CENTER_ALIGNMENT);
  display.updateWindow(22, 30,  222,  90, true);
  display.drawBitmap(Whiteboard, 22, 31,  208, 60, GxEPD_BLACK);
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

void getTimeFromNTP(String format)
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  if (format == "EU") {
    // tm_year: Years since 1900
    Date[0] = timeinfo.tm_mday / 10 % 10 + '0';
    Date[1] = timeinfo.tm_mday % 10 + '0';
    Date[2] = '/';
    Date[3] = (timeinfo.tm_mon + 1) / 10 % 10 + '0';
    Date[4] = (timeinfo.tm_mon + 1) % 10 + '0';
    Date[5] = '/';
    Date[6] = (timeinfo.tm_year + 1900) / 1000 % 10  + '0';
    Date[7] = (timeinfo.tm_year + 1900) / 100 % 10  + '0';
    Date[8] = (timeinfo.tm_year - 100) / 10 % 10 + '0';
    Date[9] = (timeinfo.tm_year - 100) % 10 + '0';
    
    Time[0] = timeinfo.tm_hour / 10 % 10 + '0';
    Time[1] = timeinfo.tm_hour % 10 + '0';
    Time[3] = timeinfo.tm_min / 10 % 10 + '0';
    Time[4] = timeinfo.tm_min % 10 + '0';
    Time[6] = timeinfo.tm_sec / 10 % 10 + '0';
    Time[7] = timeinfo.tm_sec % 10 + '0';
    }
  else {
    // tm_year: Years since 1900
    Date[0] = (timeinfo.tm_year + 1900) / 1000 % 10  + '0';
    Date[1] = (timeinfo.tm_year + 1900) / 100 % 10  + '0';
    Date[2] = (timeinfo.tm_year - 100) / 10 % 10 + '0';
    Date[3] = (timeinfo.tm_year - 100) % 10 + '0';
    Date[4] = '/';
    Date[5] = (timeinfo.tm_mon + 1) / 10 % 10 + '0';
    Date[6] = (timeinfo.tm_mon + 1) % 10 + '0';
    Date[7] = '/';
    Date[8] = timeinfo.tm_mday / 10 % 10 + '0';
    Date[9] = timeinfo.tm_mday % 10 + '0';
  
    Time[0] = timeinfo.tm_hour / 10 % 10 + '0';
    Time[1] = timeinfo.tm_hour % 10 + '0';
    Time[3] = timeinfo.tm_min / 10 % 10 + '0';
    Time[4] = timeinfo.tm_min % 10 + '0';
    Time[6] = timeinfo.tm_sec / 10 % 10 + '0';
    Time[7] = timeinfo.tm_sec % 10 + '0';
  }

  Serial.println(Date);
  Serial.println(Time);
  Serial.println(" ");
}
