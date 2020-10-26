#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#define VFD_DATA  13
#define VFD_LATCH 12
#define VFD_CLOCK 14

int vfdNumbers[] = {
  B00000011,  //0
  B10011111,  //1
  B00100101,  //2
  B00001101,  //3
  B10011001,  //4
  B01001001,  //5
  B01000001,  //6
  B00011111,  //7
  B00000001,  //8
  B00001001
}; //9


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void clearDisplay() {
  digitalWrite(VFD_CLOCK, LOW);
  digitalWrite(VFD_LATCH, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(VFD_DATA, VFD_CLOCK, LSBFIRST, B11111111);
  }
  digitalWrite(VFD_LATCH, HIGH);
}

void testDisplay() {
  digitalWrite(VFD_CLOCK, LOW);
  digitalWrite(VFD_LATCH, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(VFD_DATA, VFD_CLOCK, LSBFIRST, B00000000);
  }
  digitalWrite(VFD_LATCH, HIGH);
}

void sendDisplay(int mess[]) {
  digitalWrite(VFD_CLOCK, LOW);
  digitalWrite(VFD_LATCH, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(VFD_DATA, VFD_CLOCK, LSBFIRST, mess[i]);
  }
  digitalWrite(VFD_LATCH, HIGH);
}

void setup() {
  Serial.begin(115200);

  // Setup WifiManager so we can change SSIDs
  WiFiManager wifiManager;
  wifiManager.autoConnect("VFDClock");

  // Setup our display outputs
  pinMode(VFD_LATCH, OUTPUT);
  pinMode(VFD_CLOCK, OUTPUT);
  pinMode(VFD_DATA, OUTPUT);
  clearDisplay();

  // Setup our NTP Client for getting the time
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
}

void displayTime() {
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  int timeDisplay[6];

  timeDisplay[0] = vfdNumbers[(currentHour / 10)];
  timeDisplay[1] = vfdNumbers[(currentHour % 10)];
  timeDisplay[2] = vfdNumbers[(currentMinute / 10)];
  timeDisplay[3] = vfdNumbers[(currentMinute % 10)];
  timeDisplay[4] = vfdNumbers[(currentSecond / 10)];
  timeDisplay[5] = vfdNumbers[(currentSecond % 10)];

  sendDisplay(timeDisplay);

}

void displayDate() {
  
   timeClient.update();

   time_t rawtime=timeClient.getEpochTime();
   struct tm * ti;
   ti = localtime(&rawtime);
   int currentYear = ti->tm_year - 100; 
   int currentMonth = (ti->tm_mon + 1) < 10 ? 0 + (ti->tm_mon + 1) : (ti->tm_mon + 1);
   int currentDate = (ti->tm_mday) < 10 ? 0 + (ti->tm_mday) : (ti->tm_mday);
   
   Serial.print("Current Date: ");
   Serial.print(currentYear);   
   Serial.print("/");
   Serial.print(currentMonth);
   Serial.print("/");
   Serial.println(currentDate);
   
   int dateDisplay[6];
   dateDisplay[0] = vfdNumbers[(currentMonth / 10)];
   dateDisplay[1] = vfdNumbers[(currentMonth % 10)];
   dateDisplay[2] = vfdNumbers[(currentDate / 10)];
   dateDisplay[3] = vfdNumbers[(currentDate % 10)];
   dateDisplay[4] = vfdNumbers[(currentYear / 10)];
   dateDisplay[5] = vfdNumbers[(currentYear % 10)];

   sendDisplay(dateDisplay);
}

void loop() {
  // put your main code here, to run repeatedly:
  displayTime();
  delay(500);
}
