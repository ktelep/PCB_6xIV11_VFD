#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <EEPROM.h>

#define VFD_LATCH 12
#define VFD_DATA  13
#define VFD_CLOCK 14
#define DATE_BUTTON 15
#define HR24_BUTTON 16

// EEPROM Starting Address
uint addr = 0;

// Some globals
bool hourMode;
bool showingTime = true;   // Are we showing time or displayign stuff
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
std::unique_ptr<ESP8266WebServer> server;


// Struct for config data
struct {
  char valid[6] = "valid"; 
  int tzoffset = 0;        // timezone offset
  bool hourMode = true;    // 24 or 12 hour mode
} config_data;


String homepage = "<html><head><title>VFD Clock</title>"
                  "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css' integrity='sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z' crossorigin='anonymous'>"
                  "<script src='https://code.jquery.com/jquery-3.5.1.min.js' integrity='sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=' crossorigin='anonymous'></script>"
                  "<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js' integrity='sha384-B4gt1jrGC7Jh4AgTPSdUtOBvfO8shuf57BaghqFfPlYxofvL8/KUEfYiJOMMV+rV' crossorigin='anonymous'></script>"
                  "<style>"
                  ".but {  height: 300px; padding-top: 25px; }"
                  ".but-label { vertical-align: middle; font-size: 75px; }"
                  ".label { font-size:75px; }"
                  ".custom { font-size: 75px; }"
                  ".radio { width: 50px; height:50px; }"
                  "input { text-align: center; }"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<div class='container'>"
                  "    <div class='row'>"
                  "        <div class='col-md-12'>"
                  "            <div class='row'>"
                  "                <div class='col-6'>"
                  "                    <a href='/date' class='but btn btn-info btn-large btn-block m-2'><span class='but-label'>Show<br>Date</span></a>"
                  "                    <a href='/time' class='but btn btn-info btn-large btn-block m-2'><span class='but-label'>Show<br>Time</span></a>"
                  "                </div>"
                  "                <div class='col-6'>"
                  "                    <a href='/test' class='but btn btn-info btn-large btn-block m-2'><span class='but-label'>Test<br>Display</span></a>"
                  "                    <a href='/clear' class='but btn btn-info btn-large btn-block m-2'><span class='but-label'>Clear<br>Display</span></a>"
                  "                </div>"
                  "            </div>"
                  "            <div class='row'>"
                  "                <div class='col-12 text-center card p-3 bg-light'>"
                  "                    <span class='custom'>Display Value</span>"
                  "                    <form action='/disp'>"
                  "                        <div class='form-group row'>"
                  "                            <div class='col-12'>"
                  "                                <input type=text name=val id=val class='form-control custom' placeholder='123456'><br>"
                  "                                <div class='form-check form-check-inline'>"
                  "                                    <input class='form-check-input radio' type='radio' name='just' value='left' id='radiopip1' checked>"
                  "                                    <label class='custom form-check-label' for='radiopip1'>Left</label>"
                  "                                    <input class='form-check-input radio' type='radio' name='just' value='center' id='radiopip2'>"
                  "                                    <label class='custom form-check-label' for='radiopip2'>Center</label>"
                  "                                    <input class='form-check-input radio' type='radio' name='just' value='right' id='radiopip3'>"
                  "                                    <label class='custom form-check-label' for='radiopip3'>Right</label><br>"
                  "                                </div>"
                  "                                <input type=submit class='custom btn btn-info btn-large btn-block' value='Set'>"
                  "                            </div>"
                  "                        </div>"                  
                  "                    </form>"
                  "                </div>"
                  "           </div>"
                  "            <div class='row'>"
                  "                <div class='col-12 text-center card p-3 bg-light'>"
                  "                    <span class='custom'>Timezone Offset</span>"
                  "                    <form action='/tzset'>"
                  "                        <div class='form-group row'>"
                  "                            <div class='col-12'>"
                  "                                <input type=text name=val id=val class='form-control custom' placeholder='-10000'><br>"
                  "                                <input type=submit class='custom btn btn-info btn-large btn-block' value='Set'>"
                  "                            </div>"
                  "                        </div>"                  
                  "                    </form>"
                  "                </div>"
                  "           </div>"
                  "       </div>"
                  "    </div>"
                  "</body>";
                  
// output VFD numbers
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
  B00001001,  //9
  B11111111   // BLANK
};

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

void displayTime() {
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  int timeDisplay[6];

  if (hourMode == true) {
    if (currentHour > 12) {
      currentHour = currentHour - 12;
    }
  }

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

  time_t rawtime = timeClient.getEpochTime();
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

void displayIP() {
  IPAddress localAddress = WiFi.localIP();

  for (int i = 0; i < 4; i++) {

    int oct = localAddress[i];
    int dig[6];

    dig[0] = 0xff;
    dig[1] = vfdNumbers[(oct / 100)];
    dig[2] = vfdNumbers[(oct % 100) / 10];
    dig[3] = vfdNumbers[(oct % 100) % 10];
    dig[4] = 0xff;
    dig[5] = 0xff;

    sendDisplay(dig);
    delay(1000);
  }
}

void checkButtons() {

  if ((digitalRead(HR24_BUTTON) == HIGH) && (digitalRead(DATE_BUTTON) == HIGH)) {
    displayIP();
  }
  if (digitalRead(HR24_BUTTON) == HIGH) {   // Flip between 24 and 12 hour mode
    hourMode = !hourMode;
    if (hourMode == true) {
      int clockMode[6] = {0xFF, 0xFF, vfdNumbers[1], vfdNumbers[2], 0xFF, 0xFF};
      sendDisplay(clockMode);
    } else {
      int clockMode[6] = {0xFF, 0xFF, vfdNumbers[2], vfdNumbers[4], 0xFF, 0xFF};
      sendDisplay(clockMode);
    }
    config_data.hourMode = hourMode;
    EEPROM.put(addr, config_data);
    EEPROM.commit();
    delay(1000);
  }

  if (digitalRead(DATE_BUTTON) == HIGH) {   // Show the date for 3 seconds
    displayDate();
    delay(2000);
    return;
  }
}

void sendHomepage() {
  server->send(200, "text/html", homepage);
}

void handleRoot() {
  sendHomepage();
}

void handleDisplay() {
  int display[6] = {255,255,255,255,255,255};
  String displayVal = server->arg("val");
  String justVal = server->arg("just");
  Serial.println(displayVal);

  if ((justVal.length() == 0) || (justVal == "left")) {
      // Left Justified
      for(int i=0; i <6; i++) {
        if (i > displayVal.length()-1) {
          display[i] = 255;
        } else {
          display[i] = vfdNumbers[displayVal[i] - 48];  // Convert each char to it's equivalent int <magic>
        }
      }
  } else if (justVal == "right") {
      int stringPos = displayVal.length();
      stringPos--;
      for(int i=5; i >=0; i--) {
         if (stringPos < 0) {
           display[i] = 255;
           stringPos--;
         } else {
           display[i] = vfdNumbers[displayVal[stringPos] - 48];
           stringPos--;
         }
      }
  } else if (justVal == "center") {
      int startPos = (6-displayVal.length())/2;
      int stringPos = 0;
      for (int i=startPos; i < 6; i++) {
        if (stringPos < displayVal.length()){
             display[i] = vfdNumbers[displayVal[stringPos] - 48];
             stringPos++;
        }
      }
  }   
  sendDisplay(display);
  showingTime = false;
  sendHomepage();
}

void handleTest() {
  testDisplay();
  showingTime = false;
  sendHomepage();
}

void handleBlank() {
  clearDisplay();
  showingTime = false;
  sendHomepage();
}

void handleTime() {
  showingTime = true;
  sendHomepage();
}

void handleDate() {
  displayDate();
  showingTime = false;
  sendHomepage();
}

void handleConfig() {
  WiFiManager wm;
  wm.startWebPortal();
}

void handleTZ() {
  char tzInput[8];
  server->arg("val").toCharArray(tzInput,7);
  int tzVal = atoi(tzInput);
  config_data.tzoffset = tzVal;

  Serial.print("New Config Data:\n");
  Serial.print("EEPROM Check: ");
  Serial.print(config_data.valid);
  Serial.print("\n");
  Serial.print("Stored TZoffset: ");
  Serial.print(config_data.tzoffset);
  Serial.print("\n");
  Serial.print("Stored HourMode: ");
  Serial.print(config_data.hourMode);
  Serial.print("\n");
  
  EEPROM.put(addr,config_data);
  EEPROM.commit();
  timeClient.setTimeOffset(config_data.tzoffset);
  Serial.print("New TZ Value Set - ");
  Serial.print(tzVal);
  Serial.print("\n");
  sendHomepage();
}
void setup() {
  Serial.begin(115200);

  // Setup WifiManager so we can change SSIDs
  WiFiManager wifiManager;
  wifiManager.autoConnect("VFDClock");

  // Setup the Webserver
  server.reset(new ESP8266WebServer(WiFi.localIP(), 80));
  server->on("/", handleRoot);
  server->on("/disp", handleDisplay);
  server->on("/time", handleTime);
  server->on("/test", handleTest);
  server->on("/date", handleDate);
  server->on("/clear", handleBlank);
  server->on("/config", handleConfig);
  server->on("/tzset", handleTZ);
  server->begin();
  
  // Setup our display outputs
  pinMode(VFD_LATCH, OUTPUT);
  pinMode(VFD_CLOCK, OUTPUT);
  pinMode(VFD_DATA, OUTPUT);
  pinMode(DATE_BUTTON, INPUT);
  pinMode(HR24_BUTTON, INPUT);
  clearDisplay();

  // Get our info from EEPROM
  EEPROM.begin(128);   // 128k is more than enough
  EEPROM.get(addr,config_data);
  Serial.print("EEPROM Validation String: ");
  Serial.print(config_data.valid);
  Serial.print("\n");
  if (strcmp(config_data.valid, "VALID") != 0) {   // data in EEPROM is invalid, set defaults
      Serial.print("Config Data Not Valid, writing defaults");
      strncpy(config_data.valid, "VALID", 6);
      config_data.tzoffset = -15000; // default TZ offset
      config_data.hourMode = true;   // default 24 hour mode
      EEPROM.put(addr,config_data);
      EEPROM.commit();
  }

  Serial.print("EEPROM Check: ");
  Serial.print(config_data.valid);
  Serial.print("\n");
  Serial.print("Stored TZoffset: ");
  Serial.print(config_data.tzoffset);
  Serial.print("\n");
  Serial.print("Stored HourMode: ");
  Serial.print(config_data.hourMode);
  Serial.print("\n");
  
  // Setup our NTP Client for getting the time
  timeClient.begin();
  timeClient.setTimeOffset(config_data.tzoffset);
  hourMode = config_data.hourMode;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (showingTime) {
      displayTime();
      checkButtons();
  }
  server->handleClient();
}
