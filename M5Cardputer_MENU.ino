#include <M5Cardputer.h>
#include <M5Unified.h>
#include "M5Cardputer.h"
#include "M5GFX.h"

const char menuItems[][20] = {
"1.",
"2.",
"3.",
"4." 
};
 String data = ">";
int currentItem = 0;
int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);

void setup() {
    M5.begin();
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1.5);
    M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 1, RED);
    M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
    M5Cardputer.Display.setCursor(2,5);
    M5Cardputer.Display.println("Main-menu:");
    M5Cardputer.Display.setTextSize(0.9);
    M5Cardputer.Display.setCursor(2,50);
    
    M5Cardputer.Display.setTextFont(&fonts::FreeSans9pt7b);
    M5.Lcd.setTextSize(1);
    displayMenu();
}

void loop() {
M5.update();
M5Cardputer.update();
if (M5Cardputer.Keyboard.isChange()) {
if (M5Cardputer.Keyboard.isPressed()) {
  Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

  for (int i : status.word) {  
    if (i == '1') {
     currentItem = 0;
     displayMenu();
    }
    if (i == '2') {
      currentItem = 1;
    displayMenu();
    }
    if (i == '3') {
      currentItem = 2;
    displayMenu();
    }
    if (i == '4') {
      currentItem = 3;
    displayMenu();
    }
    if (i == ';') {
          currentItem++;
    if (currentItem >= totalItems) currentItem = 0;
    displayMenu();
    }
    if (i == '.') {
    currentItem++;
    if (currentItem >= totalItems) currentItem = 0;
    displayMenu();
    }
  }
  if (status.alt) {
    currentItem++;
    if (currentItem >= totalItems) currentItem = 0;
    displayMenu();
  }
  if (status.ctrl) {
    currentItem--;
    if (currentItem >= totalItems) currentItem = 0;
    displayMenu();
  }
  if (status.enter) {
    handleSelect();
  }
}
}
}

void displayMenu() {
  for (int i = 0; i < totalItems; i++) {
    if (i == currentItem) { 
      M5.Lcd.setTextColor(WHITE, RED);
    } else { 
      M5.Lcd.setTextColor(RED, WHITE); 
    }
    M5.Lcd.setCursor(3, 50 + i * 20);
    M5.Lcd.print(menuItems[i]);
  }
}

void handleSelect() {
M5Cardputer.Display.clear();
switch (currentItem) {
  case 0: 
  M5Cardputer.Display.clear();
  M5.Lcd.setTextColor(0xFFFF); 
  M5.Lcd.print("It works! Option 1");
  break;
  case 1:
  M5Cardputer.Display.clear(); 
  M5.Lcd.setTextColor(0xFFFF); 
  M5.Lcd.print("It works! Option 2");
  break;
  case 2:
  M5Cardputer.Display.clear();
  M5.Lcd.setTextColor(0xFFFF); 
  M5.Lcd.print("It works! Option 3");
  break; 
  case 3:
  M5Cardputer.Display.clear();  
  M5.Lcd.setTextColor(0xFFFF); 
  M5.Lcd.print("It works! Option 4");
  break;
}
delay(5000);
M5.Lcd.setTextColor(WHITE);
M5Cardputer.Display.clear();
setup();
}