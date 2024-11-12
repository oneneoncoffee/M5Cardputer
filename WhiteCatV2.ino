#include <M5Cardputer.h>
#include <M5Unified.h>
#include "M5GFX.h" 
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include "WiFi.h"
#include <esp_wifi.h>
#include <WebServer.h> 
#include "database.h"
#include <time.h> 
#include <SD.h> 
#include "USB.h"
#include "USBHIDKeyboard.h"

//Define HID keyboard device 
USBHIDKeyboard Keyboard; 

//Define server and port 
const char* defaultESSID = "AppleRot";
WebServer server(80); 


// Define static colors
#define ORANGE 0xFD20
#define PURPLE 0x780F
#define TEAL   0x0410
#define LIGHTGRAY 0xC618
#define DARKGRAY 0x7BEF  
#define NAVY 0x000F
#define SKYBLUE 0x867D
#define SKYEBLUE 0x867D 
#define HOTPINK 0xFB56
#define CYAN 0x07FF
#define GRAY 808080
#define color random(0xFFFFFF)
// Avatar attributes 
#define otto_width  85
#define otto_height 85
// Define IR remote 
const uint16_t kIrLedPin = 44;
IRsend irsend(kIrLedPin);

// Timer every 6 Minutes 
unsigned long previousMillis = 0; 
const unsigned long interval = 360000; 


const char menuItems[][30] = {
  "1.TV -B- Gone    ",
  "2.DICE ROLL      ",
  "3.Scan local WiFi",
  "4.My RQ codes    ",
  "5.Save to SD     ",
  "6.MORE           "
};
int currentItem = 0;
int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);

void trigger_sound() {
  M5Cardputer.Speaker.begin();
  M5Cardputer.Speaker.setVolume(655);
  M5Cardputer.Speaker.tone(440, 200);
  delay(70);
  M5Cardputer.Speaker.tone(995, 198);
  delay(60);
  M5Cardputer.Speaker.tone(660, 200);
  delay(70);
  M5Cardputer.Speaker.tone(880, 300);
  delay(79);
  M5Cardputer.Speaker.end();
}

void input_sound() {
M5Cardputer.Speaker.begin();
M5Cardputer.Speaker.setVolume(544);
M5Cardputer.Speaker.tone(1424, 260);
delay(83);
M5Cardputer.Speaker.tone(1722, 330);
delay(50); 
M5Cardputer.Speaker.end();
}

void setup() {

  auto cfg = M5.config();
  cfg.output_power = true;
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0); 
  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 1, RED);
  M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Cardputer.Display.setCursor(1, 1);
  Keyboard.begin(); 
  USB.begin(); 
  M5.Lcd.setTextColor(random(0x00FFF));
  M5.Lcd.setTextSize(0.5);
  M5Cardputer.Display.println("White CatZo0*");
  M5Cardputer.Display.setTextSize(0.9);
  M5Cardputer.Display.setCursor(2, 10);
  startAccessPoint(defaultESSID); 
  serverSetup(); 
  trigger_sound();
  M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
  M5.Lcd.setTextSize(1);
  displayMenu();
}

void loop() {
  server.handleClient();
  M5.update();
  M5Cardputer.update();
  unsigned long currentMillis = millis(); 
  if (currentMillis - previousMillis >= interval) { 
    catz();
    previousMillis = currentMillis;  
  }
  if (M5Cardputer.Keyboard.isChange()) {
    input_sound();
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      for (int i : status.word) {
        if (i == '1') {
          input_sound();
          currentItem = 0;
          displayMenu();
        }
        if (i == '2') {
          input_sound();
          currentItem = 1;
          displayMenu();
        }
        if (i == '3') {
          input_sound();
          currentItem = 2;
          displayMenu();
        }
        if (i == '4') {
          input_sound();
          currentItem = 3;
          displayMenu();
        }
        if (i == '5') {
          input_sound();
          currentItem = 4;
          displayMenu();
        }
        if (i == '6') {
          input_sound();
          currentItem = 5;
          displayMenu();
        }
          if (i == 'm') {
          input_sound();
          M5Cardputer.Display.clear();
          M5Cardputer.Display.setCursor(1,1);
          M5.Lcd.fillScreen(BLACK);
          displayMenu();
        }  
          if (i == 'v') {
          input_sound();
          M5Cardputer.Display.clear();
          M5Cardputer.Display.setCursor(1,1);
          M5.Lcd.fillScreen(BLACK);
          displayMenu();
        }

        if (i == ';') {
          input_sound();
          currentItem++;
          if (currentItem >= totalItems) currentItem = 0;
          displayMenu();
        }
        if (i == '.') {
          input_sound();
          currentItem++;
          if (currentItem >= totalItems) currentItem = 0;
          displayMenu();
        }
      if (i == 'r') {
        trigger_sound();
        M5Cardputer.Display.clear();
        M5.Lcd.fillScreen(BLACK);
        techBgone();
      }
         if (i == 'h') {
          input_sound(); 
          trigger_sound();
          hideandseek(); 
          M5Cardputer.Display.clear(); 
          trigger_sound();
          displayMenu2(); 
        }
      }
      if (status.del) {
        input_sound();
        sdremove(); 
        trigger_sound();
      }
      if (status.opt) {
      input_sound(); 
      trigger_sound(); 
      essidsdspam();
      trigger_sound(); 
      }
      if (status.fn) {
        input_sound(); 
        keyboarddo();
        trigger_sound();
      }
      if (status.alt) {
        input_sound();
        currentItem++;
        if (currentItem >= totalItems) currentItem = 0;
        displayMenu();
      }
      if (status.ctrl) {
        input_sound();
        currentItem--;
        if (currentItem >= totalItems) currentItem = 1;
        displayMenu();
      }
      if (status.enter) {
        trigger_sound();
        handleSelect();
      }
      if(M5.BtnA.pressedFor(1000)) {
        input_sound();
         for (int x = 0; x < 4; x++) {
      irsend.begin();
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("IR sending off codes %d", x + 1);
      irsend.sendSony(0x540C0, 20);
      delay(100);
      irsend.sendSony(0x54028, 20);
      delay(100); 
      irsend.sendNEC(samsungPower, 32);
      delay(100);
      irsend.sendNEC(0x20DF10EF, 32);
      delay(100);
      irsend.sendNEC(0xE0E019E6, 32);
      delay(100);
      irsend.sendSony(sonyPower, 12);
      delay(100);
      irsend.sendSony(0xA55A50AF, 32);
      delay(100);
      irsend.sendSony(0xF50, 12);
      delay(100);
      irsend.sendSony(0xA8B47, 12);
      delay(100);
      irsend.sendNEC(lgPower, 32);
      delay(100);
      irsend.sendPanasonic(panasonicPower, 32);
      delay(100);
      irsend.sendSony(0x000A5A5A, 32);
      delay(100);
      irsend.sendNEC(0xE0E040BF, 32);
      delay(100); 
      irsend.sendNEC(0x16D648B7, 32); 
      delay(100); 
      irsend.sendNEC(0xE4CD1208, 32);
      delay(100); 
    } 
     trigger_sound();
     M5.Lcd.fillScreen(BLACK);
     M5.Lcd.setCursor(0,0);
      }
    }
  server.handleClient();
}
}

float getBatteryPercentage(float voltage, float minVoltage, float maxVoltage) {
float precentage = ((voltage - minVoltage) / (maxVoltage - minVoltage)) * 100;
    return constrain(precentage, 0, 100); 
}
float getBatAVG(float voltage) {
 const float voltageLevels[] = {3.2, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2}; 
 const float percentageLevels[] = {0,10,20,30,45,60,75,85,95,100};
  if (voltage <= voltageLevels[0]) {
   return percentageLevels[0];
  } 
  if (voltage >= voltageLevels[9]) {
   return percentageLevels[9]; 
  } 
  for (int i = 0; i < 9; i++) {
    if (voltage >= voltageLevels[i] && voltage < voltageLevels[i + 1]) {
    float ratio = (voltage - voltageLevels[i]) / (voltageLevels[i + 1] - voltageLevels[i]);
    return percentageLevels[i] + ratio * (percentageLevels[i + 1] - percentageLevels[i]);  
    }
  }
}
float getBatPercentage(float voltage) {
  if (voltage >= 4.2) {
    return 100.0;
  } else if (voltage > 4.0) { 
    return 75.0 + (voltage - 4.0) * 25.0 / 0.2;
  } else if (voltage > 3.7) {
    return 25.0 + (voltage - 3.7) * 50.0 / 0.3;
  } else if (voltage > 3.4) {
    return (voltage - 3.4) * 25.0 / 0.3;
  } else {
  return 0.0;
  float precentage = ((voltage - 2100.00) / (4172.00 - 2100.00)) * 100;
  return constrain(precentage, 0, 100); 
  }
}
void batt_update() {
  float internalVoltage = M5Cardputer.Power.getBatteryVoltage();
  float internalPercentage = getBatteryPercentage(internalVoltage, 2100.00, 4172.00);
  float voltage = M5Cardputer.Power.getBatteryVoltage();
  float precentage = batteryPrecentage(voltage);
  float precentbat = getBatPercentage(voltage);
  float batavg = getBatAVG(voltage); 
  M5.Lcd.setCursor(M5.Lcd.width() - 69, 1);
  M5.Lcd.setTextSize(0.6);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.print("BAT:");
  M5.Lcd.setTextColor(ORANGE, BLACK);
  M5.Lcd.print(precentage);
  M5.Lcd.println("V");
  M5.Lcd.setCursor(M5.Lcd.width() - 88, 11);
  M5.Lcd.print(internalVoltage);
  M5.Lcd.print("V / ");
  M5.Lcd.print(internalPercentage);
  M5.Lcd.print("%");
  M5.Lcd.setCursor(M5.Lcd.width() - 88, 21);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  M5.Lcd.print("Overall:");
  M5.Lcd.setTextColor(ORANGE, BLACK);
  M5.Lcd.print(precentbat);
  M5.Lcd.println("%"); 
  M5.Lcd.setCursor(M5.Lcd.width() - 88, 31);
  M5.Lcd.setTextColor(YELLOW, BLACK); 
  M5.Lcd.print("AVG:"); 
  M5.Lcd.setTextColor(ORANGE, BLACK); 
  M5.Lcd.print(batavg); 
  M5.Lcd.print("%"); 
  M5.Lcd.setTextColor(ORANGE, BLACK); 
  M5Cardputer.Display.setTextSize(0.5);
  uint16_t cx = M5Cardputer.Display.color565(random(0,255), random(0,255),random(0,255));
  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 1, cx);
  M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Cardputer.Display.setCursor(1, 1);
  M5.Lcd.setTextColor(random(0x00FFF));
  M5Cardputer.Display.println("White CatZoO*");
  M5Cardputer.Display.setTextSize(0.9);
  M5Cardputer.Display.setCursor(2, 10);
  M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
  M5.Lcd.setTextSize(1);
  delay(10);
}

int batteryPrecentage(float voltage) {
  if (voltage > 4.2) return 4.2;
  if (voltage > 4.1) return 4.1;
  if (voltage > 4.0) return 4.0;
  if (voltage > 3.9) return 3.9;
  if (voltage > 3.8) return 3.8;
  if (voltage > 3.7) return 3.7;
  if (voltage > 3.6) return 3.6;
  if (voltage > 3.5) return 3.5;
  if (voltage > 3.4) return 3.4;
  if (voltage > 3.3) return 3.3;
  if (voltage > 3.0) return 3.0;
  return 0;
}

void displayMenu() {
  catz();
  for (int i = 0; i < totalItems; i++) {
    if (i == currentItem) {
      M5.Lcd.setTextColor(WHITE, BLACK);
    } else {
      M5.Lcd.setTextColor(RED, BLACK);
    }
    M5.Lcd.setCursor(3, 13 + i * 20);
    M5.Lcd.print(menuItems[i]);
  }
  batt_update();
}

void handleSelect() {
  M5Cardputer.Display.clear();
  switch (currentItem) {
    case 0:
      M5Cardputer.Display.clear();
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.print("                  ");
      techBgone();
      break;
    case 1:
      M5Cardputer.Display.clear();
      M5.Lcd.setTextColor(0xFFFF);
      dice();
      M5.Lcd.print("                 \n\n\n\nDICE ROLL");
      break;
    case 2:
      M5Cardputer.Display.clear();
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.printf("                 \n\n WiFi mode set..\nScanning... ");
      networkscan();
      break;
    case 3:
      M5Cardputer.Display.clear();
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.print("   ");
      RQCodex();
      break;
    case 4:
     M5Cardputer.Display.clear();
     M5.Lcd.setTextColor(0xFFFF);
     saveList(); 
     break;
    case 5:
    submenu();
    break;
  }
  delay(5000);
  M5.Lcd.setTextColor(WHITE);
  M5Cardputer.Display.clear();
  setup();
}

void techBgone() {
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(BLUE, GRAY);
  for (int x = 0; x < 4; x++) {
    M5.Lcd.setCursor(1, 50);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(random(0xFFFF));
    M5.Lcd.print("TV-B");
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 99);
    M5.Lcd.setTextColor(random(0xFFFF));
    M5.Lcd.print("GONE");
    delay(455);
  }
  irsend.begin();
  M5Cardputer.Display.clear();
  M5.Lcd.setTextSize(1);
  while (true) {
    for (uint8_t i = 0; i < sizeof(tvPowerOffCodes) / sizeof(tvPowerOffCodes[0]); i++) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("Sending code %d\n", i + 1);
      irsend.sendNEC(tvPowerOffCodes[i], 32);
      delay(500);
    }
    for (int x = 0; x < 2; x++) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("Batch sending codes %d", x + 1);
      irsend.sendNEC(samsungPower, 32);
      delay(100);
      irsend.sendNEC(0x20DF10EF, 32);
      delay(100);
      irsend.sendNEC(0xE0E019E6, 32);
      delay(100);
      irsend.sendSony(sonyPower, 12);
      delay(100);
      irsend.sendSony(0xA55A50AF, 32);
      delay(100);
      irsend.sendSony(0xF50, 12);
      delay(100);
      irsend.sendSony(0xA8B47, 12);
      delay(100);
      irsend.sendNEC(lgPower, 32);
      delay(100);
      irsend.sendPanasonic(panasonicPower, 32);
      delay(100);
    }
  }
}
#define DOT_SIZE 6
int dot[6][6][2]{
  { { 35, 35 } },
  { { 15, 15 }, { 55, 55 } },
  { { 15, 15 }, { 35, 35 }, { 55, 55 } },
  { { 15, 15 }, { 15, 55 }, { 55, 15 }, { 55, 55 } },
  { { 15, 15 }, { 15, 55 }, { 35, 35 }, { 55, 15 }, { 55, 55 } },
  { { 15, 15 }, { 15, 35 }, { 15, 55 }, { 55, 15 }, { 55, 35 }, { 55, 55 } },
};

void draw_dice(int16_t x, int16_t y, int n) {
  M5.Lcd.fillRect(x, y, 70, 70, WHITE);
  M5.Lcd.drawRect(x + 1, y + 1, 71, 71, GRAY);

  for (int d = 0; d < 6; d++) {
    if (dot[n][d][0] > 0) {
      M5.Lcd.fillCircle(x + dot[n][d][0], y + dot[n][d][1], DOT_SIZE,
                        TFT_BLACK);
    }
  }
}

void dice() {
  M5.Lcd.fillScreen(TFT_BLACK);
  delay(500);
  draw_dice(5, 5, random(0, 6));
  delay(500);
  draw_dice(85, 5, random(0, 6));
  delay(1500);
}

void networkscan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  M5Cardputer.Display.clear();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(90, 0);
  M5.Lcd.setTextColor(random(0x00FFF));
  M5.Lcd.setTextSize(0.5);
  for (int x = 0; x < 3; x++) {
    int pos = 0;
    int n = WiFi.scanNetworks();
    M5.Lcd.print("Scan done:");
    if (n == 0) {
      M5.Lcd.println("no networks found");
    } else {
      M5.Lcd.print(n);
      M5.Lcd.println(" networks found");
      delay(1900);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      int displaycount = 0;
      for (int i = 0; i < n; ++i) {
        if (displaycount == 0) {
          M5.Lcd.fillScreen(BLACK);
          M5.Lcd.setCursor(0, 0);
          M5.Lcd.setTextColor(random(0x0FFFF));
        }
        M5.Lcd.printf("%2d", i + 1);
        M5.Lcd.print(",");
        M5.Lcd.printf("%-32.32s", WiFi.SSID(i).c_str());
        M5.Lcd.print(",");
        M5.Lcd.printf("%4d", WiFi.RSSI(i));
        M5.Lcd.print(",");
        M5.Lcd.printf("%2d", WiFi.channel(i));
        M5.Lcd.print(",");
        switch (WiFi.encryptionType(i)) {
          case WIFI_AUTH_OPEN:
            M5.Lcd.print("open");
            break;
          case WIFI_AUTH_WEP:
            M5.Lcd.print("WEP");
            break;
          case WIFI_AUTH_WPA_PSK:
            M5.Lcd.print("WPA");
            break;
          case WIFI_AUTH_WPA2_PSK:
            M5.Lcd.print("WPA2");
            break;
          case WIFI_AUTH_WPA_WPA2_PSK:
            M5.Lcd.print("WPA+WPA2");
            break;
          case WIFI_AUTH_WPA2_ENTERPRISE:
            M5.Lcd.print("WPA2-EAP");
            break;
          case WIFI_AUTH_WPA3_PSK:
            M5.Lcd.print("WPA3");
            break;
          case WIFI_AUTH_WPA2_WPA3_PSK:
            M5.Lcd.print("WPA2+WPA3");
            break;
          case WIFI_AUTH_WAPI_PSK:
            M5.Lcd.print("WAPI");
            break;
          default:
            M5.Lcd.print("unknown");
        }
        displaycount++;
        if (displaycount == 10 || i == n - 10) {
          displaycount = 0;
          delay(2000);
        }
        M5.Lcd.println("");
        delay(60);
      }
    }
    M5.Lcd.println("");
    WiFi.scanDelete();
    delay(1000);
  }
  delay(1000);
  M5.Lcd.fillScreen(BLACK);
}

void RQCodex() {
  while (true) {
    M5.Lcd.setTextSize(0.7);
    M5Cardputer.Display.setRotation(2);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.setCursor(2, 4);
    M5.Lcd.println("My Source Code");
    M5.Lcd.qrcode("https://github.com/oneneoncoffee");
    M5.Lcd.setCursor(0, 220);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.println("Scan with your phone");
    M5Cardputer.update();
    M5.update();
    if (M5.BtnA.isPressed()) { break; }
    delay(100);
  }
  M5Cardputer.Display.clear();
   while (true) {
    M5.Lcd.setTextSize(0.7);
    M5Cardputer.Display.setRotation(2);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.setCursor(2, 4);
    M5.Lcd.println("My Name:");
    M5.Lcd.qrcode("Johnny Buckallew Stroud ");
    M5.Lcd.setCursor(0, 220);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.println("Scan with your phone");
    M5Cardputer.update();
    M5.update();
    if (M5.BtnA.pressedFor(1000)) { break; }
    delay(101);
  }
    M5Cardputer.Display.clear();
   while (true) {
    M5.Lcd.setTextSize(0.7);
    M5Cardputer.Display.setRotation(2);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.setCursor(2, 4);
    M5.Lcd.println("My Phone #");
    M5.Lcd.qrcode("4063172783");
    M5.Lcd.setCursor(0, 220);
    M5.Lcd.setTextColor(random(0xFFFFF));
    M5.Lcd.println("Scan with your phone");
    M5Cardputer.update();
    M5.update();
    if (M5.BtnA.pressedFor(2000)) { break; }
    delay(140);
  }
}

const char menuItems2[][30] = {
  "8.D20 dice roll",
  "9.ESSID Spam   ", 
  "11.Guard Server",
  "12.Starfield   ",
  "13.ESSID Clone ",
  "HOME"
};

int currentItem2 = 0;
int totalItems2 = sizeof(menuItems2) / sizeof(menuItems2[0]);

void displayMenu2() {
  catz(); 
  for (int i = 0; i < totalItems2; i++) {
    if (i == currentItem2) {
      M5.Lcd.setTextColor(WHITE, BLACK);
    } else {
      M5.Lcd.setTextColor(RED, BLACK);
    }
    M5.Lcd.setCursor(5, 13 + i * 20);
    M5.Lcd.print(menuItems2[i]);
  }
  batt_update();
}

void submenu() {
      M5.update();
      M5Cardputer.update();
      displayMenu2();
      M5.Lcd.setTextColor(RED);  
      M5.Lcd.setCursor(5,4);
      int x = 0;
      int y = 0;
      int q = 0;
M5Cardputer.Display.drawRect(x, y, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
M5Cardputer.Display.drawRect(x+2, y+2, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));

while(true) {
server.handleClient();
if (q == 5500) {
M5.Lcd.setTextColor(RED);  
M5.Lcd.setCursor(5,4);
M5Cardputer.Display.drawRect(x, y, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
M5Cardputer.Display.drawRect(x+2, y+2, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));
q = 0;
}
M5.update();
M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      for (int i : status.word) { 
      if (i == '8') {
      trigger_sound();
      
      displayMenu2();
      } 
      if (i == '9') {
      trigger_sound();
      dice_20();
      displayMenu2();
      }
      if (i == ';') {
          input_sound();
          currentItem2++;
          if (currentItem2 >= totalItems2) currentItem2 = 0;
          displayMenu2();
        }
        if (i == '.') {
          input_sound();
          currentItem2++;
          if (currentItem2 >= totalItems2) currentItem2 = 0;
          displayMenu2();
        }
        if (i == 'h') {
          input_sound(); 
          trigger_sound();
          hideandseek(); 
          M5Cardputer.Display.clear(); 
          trigger_sound();
          displayMenu2(); 
        }
      }
    
      if (status.alt) {
        input_sound();
        currentItem2++;
        if (currentItem2 >= totalItems2) currentItem2 = 0;
        displayMenu2();
      }
      if (status.ctrl) {
        input_sound();
        currentItem2--;
        if (currentItem2 >= totalItems2) currentItem2 = 0;
        displayMenu2();
      }
      if (status.enter) {
       trigger_sound();
       handleSelect2();
      }
     if (status.fn) {
        input_sound(); 
        keyboarddo();
        trigger_sound();
      }
      if (status.opt) {
      input_sound(); 
      trigger_sound(); 
      essidsdspam();
      trigger_sound(); 
      }
    }
  }
  if (M5.BtnA.pressedFor(1000)) { 
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0,0);
    trigger_sound();
    break; 
    }
    q++;
    server.handleClient();
}
}

void handleSelect2() {    
  M5Cardputer.Display.clear();
  switch (currentItem2) {
    case 0:
     M5.Lcd.setCursor(0,0);
     M5Cardputer.Display.setCursor(1,1);
     dice_20();
      break;
    case 1:
      M5Cardputer.Display.clear();
      M5.Lcd.setTextColor(0xFFFF);
      essidSpam();
      essidFUbar(); 
      break;
    case 2: 
      while(true) {
        servergo();  
        for (int x = 0; x<46000; ++x) {
         server.handleClient();  
        }
        server.handleClient();  
        }
      break;
    case 3:
     random_box();
     break;
    case 4:
     recall_netroll();
     break; 
    case 5:
    ESP.restart();
     break;
  }
  delay(50);
  M5.Lcd.setTextColor(WHITE);
  M5Cardputer.Display.clear();
  currentItem2 = 0;
  submenu();
}

void dice_20() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.fillScreen(BLACK);
  randomSeed(analogRead(0));
  int diceRoll = random(1, 20);
  M5Cardputer.Display.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5Cardputer.Display.setCursor(5,30); 
  M5Cardputer.Display.setTextFont(&fonts::FreeSerifBoldItalic18pt7b);
  M5Cardputer.Display.print("D20 Dice roll:");
  M5.Lcd.setTextColor(random(0xFFFFF));
  M5Cardputer.Display.setTextSize(4);
  M5Cardputer.Display.setCursor(80, 41);
  M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Cardputer.Display.printf("%d", diceRoll);
  delay(3500);
  M5Cardputer.Display.setCursor(1, 1);
  M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
  M5.Lcd.setTextSize(1);
  currentItem2 = 0;
}

void servergo() {
  M5Cardputer.Display.clear();
  M5.Lcd.setCursor(5,9); 
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_MODE_NULL); 
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP("AppleRot");
  M5.Lcd.setTextColor(random(0xFFFFF)); 
  IPAddress IP = WiFi.softAPIP();
  M5.Lcd.println(IP.toString());
  M5.Lcd.println("     Essid Name:");
  M5.Lcd.println("     AppleRot");

  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
  M5Cardputer.Display.drawRect(0+2, 0+2, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));
  M5Cardputer.Display.drawRect(0+3, 0+3, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
  M5Cardputer.Display.drawRect(0+4, 0+4, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));

for (int x=0; x>1000; ++x) { server.handleClient(); delay(50); }

while(true) {  
  for (int x = 0; x<46000; ++x) {
  server.handleClient();
  int screenWidth = M5Cardputer.Display.width();
  char* textt = "Server [OKAY] \t    ";
  int textWidth = M5Cardputer.Display.textWidth(textt);
  int positionX; 
  for(positionX = screenWidth; positionX > -textWidth; positionX--) {
  M5Cardputer.Display.clear();
	M5.Lcd.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextColor(BLACK); 
  M5Cardputer.Display.setCursor(positionX, 20);
  M5Cardputer.Display.print(textt);
  M5Cardputer.Display.setTextColor(random(0xFFFFF));
  M5Cardputer.Display.setCursor(positionX, 20);
  M5Cardputer.Display.print(textt);
  M5.Lcd.println("\n ESSID: 1applerot");
  M5.Lcd.print("IP:");
  M5.Lcd.println(IP.toString());
  delay(65);  
  server.handleClient(); 
  } 

  for(positionX = -textWidth; positionX < screenWidth; positionX++) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setTextColor(BLACK);
  M5Cardputer.Display.setCursor(positionX, 20);
  M5Cardputer.Display.print(textt);
  M5Cardputer.Display.setTextColor(random(0xFFFFF));
  M5Cardputer.Display.setCursor(positionX, 20);
  M5Cardputer.Display.print(textt);
   M5.Lcd.println("\n ESSID: 1applerot");
  M5.Lcd.print("    IP:");
  M5.Lcd.println(IP.toString());
  delay(65);
   server.handleClient(); 
  }
  server.handleClient();  
  }
  }

}
#define NSTARS 1015 
uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};
uint8_t za, zb, zc, zx;
inline uint8_t __attribute__((always_inline)) rng() {
    zx++;
    za = (za ^ zc ^ zx);
    zb = (zb + za);
    zc = (zc + ((zb >> 1) ^ za));
    return zc;
}

void random_box() {
    int screen_x = random(255);
    int screen_y = random(255);
    za = random(256);
    zb = random(256);
    zc = random(256);
    zx = random(256);
    uint8_t r, g, b;
    r=g=b=random(1, 255); 
    while(true) {
    M5Cardputer.update(); 
        if (M5.BtnA.wasPressed()) {
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.66, M5.Lcd.color565(r, g, b));  
                  }  
        if (M5.BtnA.pressedFor(1000)) {
          M5.Lcd.fillCircle(screen_x, screen_y, 0.99, M5.Lcd.color565(255, g, 255));
        } 
        if (M5.BtnA.pressedFor(9000)) { 
          M5Cardputer.Display.clear(); 
        }         
                  if (M5.BtnA.wasChangePressed()) {
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.49, M5.Lcd.color565(255, 255, 255));  
                  }
    M5.update(); 
    server.handleClient();
 unsigned long t0            = micros();
    uint8_t spawnDepthVariation = 1201;
    for (int i = 0; i < NSTARS; ++i) {
        if (sz[i] <= 1) {
            sx[i] = 160 - 120 + rng();
            sy[i] = rng();
            sz[i] = spawnDepthVariation--;
        } else {
            int old_screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 440;
            int old_screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 580;
            M5.Lcd.drawPixel(old_screen_x, old_screen_y, TFT_BLACK);   
            M5.Lcd.fillCircle(old_screen_x, old_screen_y, 0.07, TFT_BLACK);                                    
            sz[i] -= 2;
            if (sz[i] > 1) {
                int screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 440;
                int screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 580;
                if (screen_x >= 0 && screen_y >= 0 && screen_x < 580 &&
                    screen_y < 460) {
                    uint8_t r, g, b;
                    r = random(1,255); 
                    g = random(1,255); 
                    b = random(1,255) - sz[i];
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, g, b));
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.07, M5.Lcd.color565(r, g, b));
                    M5.Lcd.fillCircle(screen_x, screen_y, random(0.01, 0.11), M5.Lcd.color565(r, g, b));  
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(200, 222, b));  
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(200, g, 244));
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, 222, 255));
                    M5.update();
                    M5Cardputer.update(); 
                  if (M5.BtnA.wasPressed()) {
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, g, b));    
                    M5.Lcd.drawCircle(screen_x, screen_y, 0.66, M5.Lcd.color565(r, g, b));
                    catz();  
                  }  
                    if (M5.BtnA.pressedFor(10000)) { 
                      M5Cardputer.Display.clear(); 
                    }  
                  if (M5.BtnA.wasChangePressed()) {
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(200, 222, b));    
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.99, M5.Lcd.color565(255, 255, 255));
                
                  }

                } else
                    sz[i] = 0; 
                    uint8_t r, g, b;
                    r = random(255); 
                    g = random(1,255); 
                    b = random(1,255) - sz[i];
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, g, b));    
                    M5.Lcd.fillCircle(screen_x, screen_y, random(0.01, 0.20), M5.Lcd.color565(r, g, b));
                    M5.update();
                    M5Cardputer.update(); 
                      if (M5.BtnA.wasPressed()) {
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, g, b));    
                    M5.Lcd.drawCircle(screen_x, screen_y, 0.22, M5.Lcd.color565(r, g, b));  
                  }  
                    if (M5.BtnA.pressedFor(1000)) { 
                    catz();   
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.70, M5.Lcd.color565(r, g, b));                    
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(200, 222, b)); 
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(r, 222, 255));
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(250, g, 244));  
                  }  
                  if (M5.BtnA.wasChangePressed()) {
                    M5.Lcd.drawPixel(screen_x, screen_y, M5.Lcd.color565(200, 222, b));    
                    M5.Lcd.fillCircle(screen_x, screen_y, 0.32, M5.Lcd.color565(255, 255, 255));  
                  }  
            }
        }
    }
    unsigned long t1 = micros();
    server.handleClient();
 }
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
switch (encryptionType) {
  case WIFI_AUTH_OPEN: return "*OPEN";
  case WIFI_AUTH_WEP: return "WEP";
  case WIFI_AUTH_WPA_PSK: return "WPA/PSK";
  case WIFI_AUTH_WPA2_PSK: return "WPA2/PSK";
  case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2/ENTERPRISE";
  case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3/PSK";
  case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2/PSK";
  case WIFI_AUTH_WPA3_PSK: return "WPA3/PSK";
  case WIFI_AUTH_WAPI_PSK: return "WAPI/PSK"; 
  default: return "Unknown";
}
}
void serverSetup() {
  server.on("/", handleRoot);
  server.on("/setessid", HTTP_POST, []() {
  if(server.hasArg("essid")) {
    String newESSID = server.arg("essid");
    server.send(200, "text/html", "<html><body><h1>ESSID UPDATED</h1><p>Server new name:"+newESSID+"</p></body></html>");
    changeESSID(newESSID.c_str());

   } else {
    server.send(400, "text/plain", "Error: ESSID not provided");
   }
  });
  server.begin();
  server.handleClient();
}

void changeESSID(const char* newESSID) {
 WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_MODE_NULL); 
  WiFi.mode(WIFI_AP_STA);   
  WiFi.softAPdisconnect(true);
startAccessPoint(newESSID);
}

void handleRoot() {
  String html = "<html>";
  html +="<head><title>Rotten Apple Server</title></head>";
  html +="<body>";
  html +="<h1>Update Server ESSID</h1>";
  html +="<form action='/setessid' method='POST'>";
  html +="ESSID: <input type='text' name='essid'><br>";
  html +="<input type='submit' value='update'>";
  html +="</form><br/>"; 
  html +="<h2>Available local networks</h2>";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    html +="<p>No networks found at this time! Searching for more information.</p>";
  } else {
    html +="<ul>";
    for (int i = 0; i < n; ++i) {
      String encryption = getEncryptionType(WiFi.encryptionType(i));
      html += "<li>"+String(WiFi.SSID(i)) + " ("+WiFi.RSSI(i)+" dBm) Encryption: [<i>"+encryption+"</i>]</li>";
    }
    html +="</ul>";
  }
  html +="</body></html>";
  server.send(200, "text/html", html);
}

void startAccessPoint(const char* default_essid) {
   WiFi.disconnect(true);
  delay(100);
  WiFi.begin();
  delay(100);
  WiFi.mode(WIFI_MODE_NULL); 
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP(default_essid);
} 

void saveList() {
 if(!SD.begin()) { 
  M5Cardputer.Display.setTextSize(0.7);
  M5Cardputer.Display.setCursor(0,1); 
  M5Cardputer.Display.setTextColor(ORANGE); 
  M5Cardputer.Display.println("SD card initialization failed!");
  delay(1000);
  SD_error("SD card initialization error!");
  return; 
 }
 M5Cardputer.Display.clear();
 M5Cardputer.Display.setTextSize(0.7);
 M5Cardputer.Display.setTextFont(&fonts::FreeSerifBoldItalic18pt7b); 
 M5Cardputer.Display.setTextColor(GREEN);
 M5Cardputer.Display.println("SD card initialized!"); 
 WiFi.mode(WIFI_STA); 
 WiFi.disconnect(); 
 delay(1000);
 String filename = "/essids.dat";    //"/essids_"+String(millis())+".txt"; 
 M5Cardputer.Display.clear();
 M5Cardputer.Display.setCursor(1,1);
 M5Cardputer.Display.print("Saveing to ");
 M5Cardputer.Display.println(filename); 
 File fp = SD.open(filename, FILE_WRITE);
 if (!fp) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(1,1); 
  M5Cardputer.Display.println("Failed to open file for for writing.");
  delay(1000); 
  SD_error("Failed to open file for for writing.");
  return; 
 }
 int n = WiFi.scanNetworks();
 if (n == 0) {
  fp.println(" ");
 } else { 
 for (int i = 0; i < n; i++) {
  String ssid = WiFi.SSID(i);
  M5Cardputer.Display.println(ssid); 
  fp.println(ssid); 
  delay(100); 
  M5Cardputer.Display.clear(); 
  M5Cardputer.Display.setCursor(1,1); 
 }
 }
 fp.close(); 
 M5Cardputer.Display.clear(); 
 M5Cardputer.Display.print("File saved to ");
 M5Cardputer.Display.println(filename); 
 WiFi.disconnect();
}


void catz() {
  int intValue = random(0, 12);
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  switch(intValue) {
  case 0: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, joyjoy, otto_width, otto_height, CYAN);
  delay(54);
  break;
  case 1:
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, oddball, otto_width, otto_height, TEAL);
  delay(54);
  break;
  case 2: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, applejax, otto_width, otto_height, random(0xFFFF));
  delay(54);
  break;
 case 3: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, bluebell, otto_width, otto_height, SKYEBLUE);
  delay(54);
  break; 
  case 4:  
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, PURPLE);
  delay(54);
  break;
  case 5:
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, joyjoy, otto_width, otto_height, RED);
  delay(54);
  break;
  case 6:  
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, happy, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, happy, otto_width, otto_height, HOTPINK);
  delay(54);
  break;
  case 7: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, kissme, otto_width, otto_height, ORANGE);
  break; 
  case 8: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, madotto, otto_width, otto_height, NAVY);
  break; 
  case 9:  
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, warthog, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, warthog, otto_width, otto_height, GRAY);
  break; 
  case 10:  
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, notgood, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, notgood, otto_width, otto_height, HOTPINK);
  break;
  case 11: 
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, happy, otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, happy, otto_width, otto_height, LIGHTGRAY);
  break; 
  
  default:   
  M5.Lcd.fillRect(142, 33,otto_width, otto_height, BLACK);
  M5.Lcd.drawXBitmap(142, 33, otto_bits, otto_width, otto_height, SKYBLUE);
  delay(54);
  break;
 }
}
int currentIndex = 0;
const int totalESSIDs = sizeof(essidNames) / sizeof(essidNames[0]);

void essidSpam() {
WiFi.disconnect();
WiFi.mode(WIFI_MODE_NULL); 
WiFi.mode(WIFI_AP);
changeESSID(); 
}

void essidFUbar() {
 WiFi.disconnect();
WiFi.mode(WIFI_MODE_NULL); 
WiFi.mode(WIFI_AP);

while(true) {
catz();
String randomESSID = generateRandomESSID();
WiFi.softAP(randomESSID.c_str());
    M5.Lcd.setTextColor(ORANGE, BLACK); 
    M5.Lcd.setCursor(0, 10); 
    M5.Lcd.println("Broadcasting..");
    M5.Lcd.printf("%s\n", randomESSID);
delay(random(510, 1000));
 String essid1 = essidNames[currentIndex];
 WiFi.softAP(essid1.c_str()); 
 M5.Lcd.printf("%s\n", essid1.c_str()); 
 currentIndex = (currentIndex + 1) % totalESSIDs; 
 delay(random(510, 1000));

  if(M5.BtnA.pressedFor(4000)) { break; } 
  if(M5.BtnB.pressedFor(5000)) { break; } 
}
}

void essidsdspam() {
M5Cardputer.Display.clear(); 
WiFi.disconnect();
WiFi.mode(WIFI_MODE_NULL); 
WiFi.mode(WIFI_AP);
 if(!SD.begin()) { 
  M5Cardputer.Display.setTextSize(0.7);
  M5Cardputer.Display.setCursor(0,1); 
  M5Cardputer.Display.setTextColor(ORANGE); 
  M5Cardputer.Display.println("SD card initialization failed!");
  delay(1000);
  SD_error("SD card initialization error!");
  return; 
 }
  File fpx = SD.open("/essids_list.txt", FILE_READ); 
  if (fpx) {
     while(fpx.available()) {
      M5Cardputer.Display.clear(); 
      M5.Lcd.println("Broadcasting:");
      String essidx = fpx.readStringUntil('\n');
      M5.Lcd.setCursor(2,30);
      M5.Lcd.println(essidx.c_str());
      WiFi.softAP(essidx.c_str());
      delay(1755);  
     }
     fpx.close();     
  } else {
    SD_error("Failed to open essid_list.txt");
  }
  M5Cardputer.Display.clear(); 
}

void hideandseek() {
M5Cardputer.Display.clear(); 
WiFi.disconnect();
WiFi.mode(WIFI_MODE_NULL); 
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
esp_wifi_init(&cfg);
esp_wifi_set_mode(WIFI_MODE_AP);
esp_wifi_start();
while (true) {
  M5Cardputer.Display.clear(); 
  catz();
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("Sending anonymous packets.");
  String essid1 = essidNames[currentIndex];
  M5Cardputer.Display.clear(); 
  catz();  
  M5.Lcd.setCursor(0, 10); 
  M5.Lcd.printf("%s\n", essid1.c_str()); 
  currentIndex = (currentIndex + 1) % totalESSIDs; 
  delay(100);
  broadcastBeacon(essid1.c_str(), 1);
  delay(300);
  broadcastBeacon(essid1.c_str(), random(1, 14));
  delay(300);
const char* ssidList[] = {"A1B2","B1A2","C2A3", "D3A1","E1F1","F3G1","G1F7"};
const uint8_t numSSIDs = sizeof(ssidList) / sizeof(ssidList[0]); 
  for(uint8_t i = 0; i < numSSIDs; i++) {
  broadcastBeacon(ssidList[i], 1);
  delay(100);
  }
  
  if(M5.BtnA.pressedFor(1000)) { break; }
  if(M5.BtnA.wasPressed()) { break; } 
  if(M5.BtnB.pressedFor(1000)) { break; } 
  delay(300);
}
}

void broadcastBeacon(const char* ssid, uint8_t channel) {
  uint8_t beaconPacket[128] = {
  0x80, 0x80,
  0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x18, 0xfe, 0x34, 0x12, 0x56, 0x78,
  0x18, 0xfe, 0x34, 0x12, 0x56, 0x78,
  0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x64, 0x00,
  0x01, 0x04
  };

  uint8_t ssidLen = strlen(ssid);
  beaconPacket[38] = 0x00;
  beaconPacket[39] = ssidLen;
  memcpy(&beaconPacket[40], ssid, ssidLen);
  uint8_t supportedRates[] = { 0x01, 0x08, 0x82, 0x84, 0x8B, 0x96 };
  memcpy(&beaconPacket[40 + ssidLen], supportedRates, sizeof(supportedRates));
  beaconPacket[46 + ssidLen] = 0x03;
  beaconPacket[47 + ssidLen] = 0x01;
  beaconPacket[48 + ssidLen] = channel;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_AP, beaconPacket, 49 + ssidLen, false);

}

String generateRandomESSID() {
  String essid = "";
  WiFi.softAP(essid.c_str()); 
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde-fghijklmnopqrstuvwxyz0123456789!@#$%&*()[]{}:|.~";
  for (int i = 0; i < 11; i++) {
    essid += charset[random(0, sizeof(charset) - 1)];
  }
  return essid;
}

void changeESSID() {
  int flag = 1; 
  while(true) {
  catz();
  String essid = essidNames[currentIndex];
  WiFi.softAP(essid.c_str()); 
  if (flag == 1) {
  M5.Lcd.setTextColor(WHITE, BLACK); 
  M5.Lcd.setCursor(0, 10); 
  M5.Lcd.println("Broadcasting.."); 
  M5.Lcd.printf("%s", essid.c_str()); 
  flag = 0; 
  } else {
    flag = 1; 
    M5.Lcd.setTextColor(ORANGE, BLACK); 
    M5.Lcd.setCursor(0, 10); 
    M5.Lcd.println("Broadcasting.."); 
    M5.Lcd.printf("%s", essid.c_str()); 
  }
  currentIndex = (currentIndex + 1) % totalESSIDs; 
  delay(10000); 
  M5.update();
  if(M5.BtnA.pressedFor(4000)) { break; } 
  if(M5.BtnB.pressedFor(5000)) { break; } 
  }
}

void recall_netroll() {
File ssidFile; 
String ssidName; 
int delayTime = 3000; 
WiFi.mode(WIFI_AP); 
if(!SD.begin()) {
  M5Cardputer.Display.clear(); 
  M5.Lcd.println("SD card initialization error!"); 
  delay(1000); 
  SD_error("SD card initialization error!");
  return; 
}
ssidFile = SD.open("/essids.dat", FILE_READ); 
if(!ssidFile) {
  M5Cardputer.Display.println("Failed to open SD file!"); 
  delay(1000); 
  SD_error("SD file failed to open essids.dat!");
  while(true); 
}
M5Cardputer.Display.clear(); 
M5Cardputer.Display.setCursor(1,1); 
M5Cardputer.Display.setTextColor(ORANGE); 
M5Cardputer.Display.println("Cloneing broadcast form SD file."); 
while(true) {
  if (!ssidFile.available()) {
    ssidFile.seek(0); 
  }
  ssidName = ssidFile.readStringUntil('\n'); 
  ssidName.trim(); 
  if (ssidName.length() == 0) {
    ssidFile.close(); 
  } else {
  if (ssidName.length() > 0) {
  WiFi.softAP(ssidName.c_str());
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(1,1); 
  M5.Lcd.setTextColor(random(0xFFff));  
  M5Cardputer.Display.println("Braodcasting essid:"); 
  M5Cardputer.Display.println(ssidName); 
  delay(delayTime);    
  }
 }
 if(M5.BtnA.pressedFor(1000)) { break; }
}
}


void keyboarddo() {
  if(!SD.begin()) {
  M5Cardputer.Display.clear(); 
  M5.Lcd.println("SD card initialization error!"); 
  delay(1000); 
  SD_error("SD card initialization error!");
  return; 
}
  File makeblank; 
  makeblank = SD.open("/keylogger.log", FILE_WRITE); 
  makeblank.println("Keylogger.log ready:"); 
  makeblank.close(); 
  File Keylogger; 
       Keylogger = SD.open("/keylogger.log", FILE_WRITE); 

while(true) {
    M5Cardputer.update();
    // max press 3 button at the same time
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            // for (auto i : status.word) {
            //     Keyboard.press(i);
            // }
            KeyReport report = {0};
            report.modifiers = status.modifiers;
            uint8_t index    = 0;
            for (auto i : status.hid_keys) {
                report.keys[index] = i;
                index++;
                if (index > 5) {
                    index = 5;
                }
            }
            Keyboard.sendReport(&report);
            Keyboard.releaseAll();

            // only text for display
            String keyStr = "";
            for (auto i : status.word) {
                if (keyStr != "") {
                    keyStr = keyStr + i;
                } else {
                    keyStr += i;
                }
            }
            Keylogger.println(keyStr); 
            if (keyStr.length() > 0) {
                M5Cardputer.Display.clear();
                M5Cardputer.Display.setTextColor(SKYBLUE);
                M5Cardputer.Display.setTextFont(&fonts::Roboto_Thin_24);
                M5Cardputer.Display.setTextSize(1.1); 
                M5Cardputer.Display.setCursor(33,1);
                M5Cardputer.Display.setTextDatum(middle_center);
                M5Cardputer.Display.drawString(
                    keyStr, M5Cardputer.Display.width() / 2,
                    M5Cardputer.Display.height() / 2);
            }

        } else {
            M5Cardputer.Display.clear();
            M5.Lcd.setCursor(1,1);
            M5.Lcd.setTextSize(0.88);
            M5.Lcd.setTextColor(ORANGE);  
            M5Cardputer.Display.setTextFont(&fonts::Yellowtail_32);
            M5Cardputer.Display.setTextDatum(top_left);
            M5Cardputer.Display.drawString("USB KEYBOARD",1,1);
        }
    }
    if (M5.BtnA.wasPressed()) { 
      Keylogger.close(); 
      M5.Lcd.fillScreen(BLACK); 
      trigger_sound();
      M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
      M5.Lcd.setTextSize(1);
      break; 
      }
    if (M5.BtnA.pressedFor(1000)) {
      Keylogger.close();  
      M5.Lcd.fillScreen(BLACK);
      trigger_sound();
      M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
      M5.Lcd.setTextSize(1);
      break; 
      }
}
}

void SD_error(const char*ErrorType) {
  M5Cardputer.Display.clear(); 
  M5.Lcd.fillScreen(BLUE); 
  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width() / 1, M5Cardputer.Display.height() / 1, TFT_WHITE);
  M5Cardputer.Display.setTextSize(1); 
  M5Cardputer.Display.setCursor(1,1);
  M5Cardputer.Display.setTextColor(ORANGE); 
  M5Cardputer.Display.println(ErrorType); 
  delay(30); 
     M5.update();
  while(true) {     
    if(M5.BtnA.pressedFor(3000)) { break; }
    if(M5.BtnA.wasPressed()) { break; }
  }
}

void sdremove() {
    M5Cardputer.Display.clear(); 
  M5.Lcd.fillScreen(BLACK); 
  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width() / 1, M5Cardputer.Display.height() / 1, TFT_WHITE);
  M5Cardputer.Display.setTextSize(1); 
  M5Cardputer.Display.setCursor(1,1);
  M5Cardputer.Display.setTextColor(ORANGE); 
  if (!SD.begin()) {
    SD_error("SD card initialization failed!"); 
  }

  M5.Lcd.setCursor(1,1); 
  M5.Lcd.println("SD card initialized.");
  M5.Lcd.println("Reseting files..");
  const char* fp1 = "/essids.dat"; 
  const char* fp2 = "/keylogger.log"; 
  if (SD.exists(fp1)) {
    if (SD.remove(fp1)) {
      M5.Lcd.println("File removed essids.dat"); 
    } else {
      M5.Lcd.println("File not found!");
    }
  } else {
     M5.Lcd.println("File does not exist."); 
  }
 if (SD.exists(fp2)) {
    if (SD.remove(fp2)) {
      M5.Lcd.println("File removed keylogger.log"); 
    } else {
      M5.Lcd.println("File not found!");
    }
  } else {
     M5.Lcd.println("File does not exist."); 
  }
  File new_file1 = SD.open(fp1, FILE_WRITE); 
  if (new_file1) {
      new_file1.println(" "); 
      new_file1.println(" "); 
      new_file1.close(); 
  } else {
    SD_error("Failed to create blank file."); 
  }
  File new_file2 = SD.open(fp2, FILE_WRITE); 
  if (new_file2) {
      new_file2.println(" "); 
      new_file2.println(" "); 
      new_file2.close(); 
  } else {
    SD_error("Failed to create blank file."); 
  }
 M5.Lcd.println("[DONE]"); 
 delay(2000); 
 M5.Lcd.fillScreen(BLACK); 
}
