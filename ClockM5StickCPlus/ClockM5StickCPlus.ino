#include "M5StickCPlus.h"
#include "AXP192.h"
#include "7seg70.h"
#include "ani.h"
#include <WiFi.h>
#include "time.h"
#include "wificonfig.h"
#include "timeconfig.h"


#define year 2022   // set the year
#define month 4     // set the month
#define date 22     // set the date
#define weekday 5   // set the weekday in number 1=Monday, 2=Tuesday, 3=Wednesday, 4=Thursday, 5=Friday, 6=Saturday, 7=Sunday
#define hour 18     // set the hour 
#define minute 05   // set the minute
#define second 0    // set the seconds

#define grey 0x65DB

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
int bright[5] = {1, 8, 9, 10, 12};

int H = 0;
int M = 0;

String ho = "";
String mi = "";
String se = "";

String days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

int c = 0;
int pres = 0;
bool spavanje = 0;
bool inv = 0;

int frame = 0;

// Syncing time from NTP Server
void timeSync() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(20, 15);
  M5.Lcd.print("Connect WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(20, 15);
  M5.Lcd.println("Connected");
  // Set ntp time to local
  configTime(timeZone, 0, ntpServer);

  // Get local time
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    // Set RTC time
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Hours   = timeInfo.tm_hour;
    TimeStruct.Minutes = timeInfo.tm_min;
    TimeStruct.Seconds = timeInfo.tm_sec;
    M5.Rtc.SetTime(&TimeStruct);

    RTC_DateTypeDef DateStruct;
    DateStruct.WeekDay = timeInfo.tm_wday;
    DateStruct.Month = timeInfo.tm_mon + 1;
    DateStruct.Date = timeInfo.tm_mday;
    DateStruct.Year = timeInfo.tm_year + 1900;
    M5.Rtc.SetData(&DateStruct);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 15);
    M5.Lcd.println("S Y N C");
    delay(500);
    M5.Lcd.fillScreen(BLACK);
    WiFi.disconnect();
    M5.Lcd.setTextSize(1);
  }
}

void setup() {
  pinMode(39, INPUT_PULLUP);
  pinMode(37, INPUT_PULLUP);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.setTextSize(1);

  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Axp.EnableCoulombcounter();
  M5.Axp.ScreenBreath(bright[0]);
  
  timeSync();

//  RTC_TimeTypeDef TimeStruct;
//
//  TimeStruct.Hours = hour;
//  TimeStruct.Minutes = minute;
//  TimeStruct.Seconds = second;
//  M5.Rtc.SetTime( & TimeStruct);
//
//  RTC_DateTypeDef DateStruct;
//  
//  DateStruct.WeekDay = weekday;
//  DateStruct.Month = month;
//  DateStruct.Date = date;
//  DateStruct.Year = year;
//  M5.Rtc.SetData( & DateStruct);
}

void loop() {
  spavanje = 0;

  if (digitalRead(37) == 0) {
    if (pres == 0) {
      pres = 1;
      c++;
      if (c > 3)
        c = 0;
      M5.Axp.ScreenBreath(bright[c]);
    }
  } else {
    pres = 0;
  }

  M5.Rtc.GetTime( & RTC_TimeStruct);
  M5.Rtc.GetData( & RTC_DateStruct);
  M5.Lcd.setCursor(0, 15);
  M5.Lcd.setTextFont(0);
  M5.Lcd.drawString(String(M5.Axp.GetBatVoltage()) + " V  ", 114, 3);
  M5.Lcd.setFreeFont( & DSEG7_Classic_Regular_64);

  if (H != int(RTC_TimeStruct.Hours) || M != int(RTC_TimeStruct.Minutes)) {
    ho = String(RTC_TimeStruct.Hours);
    if (ho.length() < 2)
      ho = "0" + ho;

    mi = String(RTC_TimeStruct.Minutes);
    if (mi.length() < 2)
      mi = "0" + mi;
    M5.Lcd.drawString(ho + ":" + mi, 2, 56);
    H = int(RTC_TimeStruct.Hours);
    M = int(RTC_TimeStruct.Minutes);
  }
  
  se = String(RTC_TimeStruct.Seconds);
  if (se.length() < 2)
    se = "0" + se;
  M5.Lcd.drawString(se, 180, 0, 4);

  M5.Lcd.drawString(days[RTC_DateStruct.WeekDay - 1] + "    ", 4, 0, 2);
  M5.Lcd.setTextColor(grey, TFT_BLACK);

  M5.Lcd.drawString(String(RTC_DateStruct.Date) + "/" + String(RTC_DateStruct.Month), 4, 20, 4);
  M5.Lcd.drawString(String(RTC_DateStruct.Year), 70, 28, 2);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  if (digitalRead(39) == LOW) {
    while (digitalRead(39) == LOW);
    M5.Lcd.invertDisplay(inv);
    inv = !inv;
  }

  M5.Lcd.pushImage(112, 12, 40, 40, ani[frame]);
  frame++;
  if (frame == 132)
    frame = 0;

  delay(12);
}
