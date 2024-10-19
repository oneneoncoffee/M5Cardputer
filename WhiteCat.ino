#include <M5Cardputer.h>
#include <M5Unified.h>
#include "M5GFX.h" 
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include "WiFi.h"
#include <WebServer.h> 
#include "USB.h"
#include "USBHIDKeyboard.h"
USBHIDKeyboard Keyboard;
WebServer server(80); 


// Define static colors
#define GRAY 808080
#define color random(0xFFFFFF)

const uint16_t kIrLedPin = 44;
IRsend irsend(kIrLedPin);

// Remote IR power codes
uint32_t samsungPower = 0xE0E040BF;
uint32_t sonyPower = 0xA90;
uint32_t lgPower = 0x20DF10EF;
uint32_t panasonicPower = 0x400401FB;
uint32_t toshibaPower = 0x2FD48B7;
uint32_t sharpPower = 0x5EA1B04F;
uint32_t philipsPower = 0x0C;
uint32_t tvPowerOffCodes[] = {
  0x20DF10EF,      //Samsung
  0xE0E019E6,      //Samsung AA56
  0xE0E0807F,      // Samsung 
  0xE0E040BF,      //LG
  0x0D5F2A,        //TCL 55P715 TV
  0x10EF38C7,      //Panasonic
  0x40040100BCBD,  // Panasonic ON/OFF
  0x40040500BCB9,  // Panasonic soundbar
  0xA55A50AF,      //Sony
  0xF50,           //Sony KDL-EX540 TV
  0xA8B47,         //Sony BD-S1500
  0x10C8E11E,      //Acer K132
  0xF7C03F,        //Philips
  0x4FB040BF,      //Toshiba
  0xB24D50AF,      //Sharp
  0x48B748B7,      //Sanyo
  0x330000FF,      //Sanyo PLV-Z4
  0x878968B7,      // Hitachi
  0x9D9A5F,        // Vizio
  0x1AE6409F,      // Hisence
  0x1FE48B7,       // JVC
  0x31CEB847,      // Mitsubishi
  0x20DF23DC,      // Daewoo
  0xF7C03F,        // Grundig
  0xA3C8D728,      // Bang & Gulfsen
  0xB14CE957,      // Funai
  0xE0E09966,      // RCA
  0x30CF13EC,      // Piooneer
  0xF7C87F,        // Thomson
  0xD7E84B1C,      // Akai
  0x20DF10EF,      // NEC ON/OFF
  0xFD2502FD,      // NEC Soundcore Infini Pro
  0x4FB30CF,       // Goodmans GDSBT1000P
  0x00FF30CF,      // NEC Smart Reveiver VX/CX
  0x00FF0CF3,      // NEC Smart Reveiver
  0x00FEA857,      // Generic VEON TV (eg model SR0322016)
  0xC8052900C,     // VU+ Duo2 CR6 Set-top Box
  0x000800FF,      // Humax HMS-1000T DVB-T2 DVR
  0x2662BA45,      // Fetch TV Mini
  0xE13E13EC,      // Hi-Fi Amp & Receivers
  0x7E817E81,      // Yamaha V1900
  0x7E817E88,      // Yamaha AV Receivers
  0x00FF708F,      // Remote AI Vacumm Cleaners (BACK TO Charging Base)
  0x800F040C,      // Windows media center remote
  0x800F840C,      // Alt windows media remote 
  0xFF3AC5FF,      // Power led strips 
  0xFFBA45FF,      // power off led strip 
  m0xEOE040BF,      // Samsung power off 
  0x5EA1A25D,      // Sharp power off 
  0x00FF629D,      // Hisense power off 
  0x20DF10EF,      // LG OLED power off
  0x100BCBDF,      // Panasonic OLED Taco-bell 
  0x32CD280D,      // Loewe OLED TV
  0xFF0020D1,      // Bang & Olufsen OLED 
  0x7F8057A8,      // Bauhn OLED
  0xA15EA25D,      // Sanyo OLED led 
  0x20DF10EF,      // Thomson OLED 
  0x40BF00FF,      // insignia OLED
};

const char menuItems[][30] = {
  "1.TV -B- Gone    ",
  "2.DICE ROLL      ",
  "3.Scan local WiFi",
  "4.My RQ codes    ",
  "5.HID Keyboard   ",
  "6.MORE           "
};
int currentItem = 0;
int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);

void trigger_sound() {
  M5Cardputer.Speaker.begin();
  M5Cardputer.Speaker.setVolume(255);
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
M5Cardputer.Speaker.setVolume(244);
M5Cardputer.Speaker.tone(1424, 260);
delay(83);
M5Cardputer.Speaker.end();
}

void setup() {
  M5.begin();
  auto cfg = M5.config();
  cfg.output_power = true;
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0); 
  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 1, RED);
  M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Cardputer.Display.setCursor(1, 1);
  M5.Lcd.setTextColor(random(0x00FFF));
  M5.Lcd.setTextSize(0.5);
  M5Cardputer.Display.println("White CatZo0*");
  M5Cardputer.Display.setTextSize(0.9);
  M5Cardputer.Display.setCursor(2, 10);
  Keyboard.begin();
  USB.begin();  
  WiFi.begin();
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("applerot", "apple");
  server.on("/", handleRoot);
  server.begin();
  trigger_sound();
  M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
  M5.Lcd.setTextSize(1);
  displayMenu();
}

void loop() {
  M5.update();
  M5Cardputer.update();
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
      irsend.sendNEC(samsungPower, 32);
      delay(500);
      irsend.sendNEC(0x20DF10EF, 32);
      delay(500);
      irsend.sendNEC(0xE0E019E6, 32);
      delay(500);
      irsend.sendSony(sonyPower, 12);
      delay(500);
      irsend.sendSony(0xA55A50AF, 32);
      delay(500);
      irsend.sendSony(0xF50, 12);
      delay(500);
      irsend.sendSony(0xA8B47, 12);
      delay(500);
      irsend.sendNEC(lgPower, 32);
      delay(500);
      irsend.sendPanasonic(panasonicPower, 32);
      delay(500);
    } 
     trigger_sound();
     M5.Lcd.fillScreen(BLACK);
     M5.Lcd.setCursor(0,0);
      }
    }
  }
  batt_update();
}
void batt_update() {
  float voltage = M5Cardputer.Power.getBatteryVoltage();
  float precentage = batteryPrecentage(voltage);
  M5.Lcd.setCursor(M5.Lcd.width() - 69, 5);
  M5.Lcd.setTextSize(0.6);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.print("BAT:");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print(precentage);
  M5.Lcd.println("%");
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
  if (voltage > 4.2) return 100;
  if (voltage > 4.1) return 90;
  if (voltage > 4.0) return 80;
  if (voltage > 3.9) return 70;
  if (voltage > 3.8) return 60;
  if (voltage > 3.7) return 50;
  if (voltage > 3.6) return 40;
  if (voltage > 3.5) return 30;
  if (voltage > 3.4) return 20;
  if (voltage > 3.3) return 10;
  if (voltage > 3.0) return 5;
  return 0;
}

void displayMenu() {
  for (int i = 0; i < totalItems; i++) {
    if (i == currentItem) {
      M5.Lcd.setTextColor(WHITE, BLACK);
    } else {
      M5.Lcd.setTextColor(RED, BLACK);
    }
    M5.Lcd.setCursor(3, 13 + i * 20);
    M5.Lcd.print(menuItems[i]);
  }
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
     keybaordArmed();
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
      delay(500);
      irsend.sendNEC(0x20DF10EF, 32);
      delay(500);
      irsend.sendNEC(0xE0E019E6, 32);
      delay(500);
      irsend.sendSony(sonyPower, 12);
      delay(500);
      irsend.sendSony(0xA55A50AF, 32);
      delay(500);
      irsend.sendSony(0xF50, 12);
      delay(500);
      irsend.sendSony(0xA8B47, 12);
      delay(500);
      irsend.sendNEC(lgPower, 32);
      delay(500);
      irsend.sendPanasonic(panasonicPower, 32);
      delay(500);
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

void keybaordArmed() {
              M5Cardputer.Display.clear();
while(true) {
            M5.Lcd.setTextColor(random(0x00FFF)); 
            M5.Lcd.setTextSize(0.8);
            M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 1, random(0xFFFFF));
            M5Cardputer.Display.setTextFont(&fonts::Yellowtail_32);
            M5Cardputer.Display.drawString("USB Keyboard", M5Cardputer.Display.width()-12, M5Cardputer.Display.height()-10);
  M5.Lcd.print("USB Keyboard");
  M5Cardputer.update();
  M5.update();
  if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
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

  if (M5.BtnA.pressedFor(1002)) { break; }
   }
  }
 }
}

const char menuItems2[][30] = {
  "8.D20 dice roll",
  "9.HID-SHELL Script",
  "10.Guard Server",
  "11.Starfield",
  "HOME"
};

int currentItem2 = 0;
int totalItems2 = sizeof(menuItems2) / sizeof(menuItems2[0]);

void displayMenu2() {
  for (int i = 0; i < totalItems2; i++) {
    if (i == currentItem2) {
      M5.Lcd.setTextColor(WHITE, GRAY);
    } else {
      M5.Lcd.setTextColor(WHITE, BLUE);
    }
    M5.Lcd.setCursor(5, 13 + i * 20);
    M5.Lcd.print(menuItems2[i]);
  }
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
      hidattack();
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

    }

  }
  if (M5.BtnA.pressedFor(1000)) { 
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0,0);
    trigger_sound();
    break; 
    }
    q++;
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
      hidattack();
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
    ESP.restart();
     break;
  }
  delay(50);
  M5.Lcd.setTextColor(WHITE);
  M5Cardputer.Display.clear();
  currentItem2 = 0;
  submenu();
}

void hidattack() {
M5.Lcd.fillScreen(BLACK);
M5.Lcd.setCursor(5,1);
M5.Lcd.setTextSize(0.9);
M5.Lcd.setTextColor(random(0xFFFFF));
M5.Lcd.println("HID-Device mode Active!");
M5.Lcd.setTextColor(random(0xFFFFF));
M5.Lcd.println("Delay set for 100ms ->");
M5.Lcd.print("Working..");
Keyboard.begin();
M5Cardputer.update();
M5.update();
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100); 
    M5.Lcd.print(".");
    Keyboard.releaseAll();
    Keyboard.print("powershell");
    Keyboard.press(KEY_ENTER);
    delay(100);
    M5.Lcd.print(".");
    Keyboard.releaseAll();
    Keyboard.print(" ");
    Keyboard.press(KEY_RETURN);
    delay(100);
    M5.Lcd.print(".");
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
    }
  }
    Keyboard.press(KEY_LEFT_GUI); // ALL newer windows 
    Keyboard.press('r');
    Keyboard.print("powershell");
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
    delay(100);
    M5.Lcd.print(".");
    Keyboard.press(KEY_LEFT_GUI); // windows xp 
    Keyboard.press('r');
    Keyboard.print("cmd.exe");
    Keyboard.press(KEY_ENTER);
    Keyboard.print("cmd");
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
    delay(100);    
    M5.Lcd.print(".");
    Keyboard.press(KEY_LEFT_GUI); // windows 95/ms-dos  
    Keyboard.press('r');
    Keyboard.print("command.com");
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
    delay(100);
    M5.Lcd.println("..Done.");
    delay(400);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(1,1);
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
  WiFi.softAP("1applerot");
  M5.Lcd.setTextColor(random(0xFFFFF)); 
  IPAddress IP = WiFi.softAPIP();
  M5.Lcd.println(IP.toString());
  M5.Lcd.println("     Essid Name:");
  M5.Lcd.println("     1applerot");

  M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
  M5Cardputer.Display.drawRect(0+2, 0+2, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));
  M5Cardputer.Display.drawRect(0+3, 0+3, M5Cardputer.Display.width() - 1, M5Cardputer.Display.height() - 1, random(1, 255));
  M5Cardputer.Display.drawRect(0+4, 0+4, M5Cardputer.Display.width() - 7, M5Cardputer.Display.height() - 7, random(1, 255));

for (int x=0; x>1000; ++x) { server.handleClient(); delay(50); }

while(true) {  
  for (int x = 0; x<46000; ++x) {
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
#define NSTARS 4424
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

    za = random(256);
    zb = random(256);
    zc = random(256);
    zx = random(256);
    while(true) {
 unsigned long t0            = micros();
    uint8_t spawnDepthVariation = 255;
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
           
                } else
                    sz[i] = 0; 
            }
        }
    }
    unsigned long t1 = micros();
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

void handleRoot() {
  String html = "<html>";
  html +="<head><title>Rotten Apple Server</title></head>";
  html +="<body><h1>Available local networks</h1>";
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
