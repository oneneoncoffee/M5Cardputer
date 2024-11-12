#include <M5Cardputer.h> 
const char* menuOptions[] = {"Option1","Option2","Option3","Option4","Option5","Option6"}; 
const int menuItems = sizeof(menuOptions) / sizeof(menuOptions[0]);
const int maxItemLength = 10; 
int menuIndex = 0;

void setup() {
   auto cfg = M5.config();
  M5Cardputer.begin(cfg); 
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.clearDisplay(); 
  M5Cardputer.Display.setTextSize(2); 
  
  displayMenu();
}

void displayMenu() {
  M5Cardputer.Display.clear(); 
  M5Cardputer.Display.setCursor(2,6); 
  M5Cardputer.Display.setTextSize(3); 
  M5Cardputer.Display.setTextColor(TFT_ORANGE, BLACK); 
  M5Cardputer.Display.println("//Main-Menu");
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.drawRoundRect(1,1,M5Cardputer.Display.width()-1,M5Cardputer.Display.height()-1, 17, TFT_GREENYELLOW);
  M5Cardputer.Display.setCursor(0,29); 
  for (int i = 0; i < menuItems; i++) {
    if (i == menuIndex) {
      M5Cardputer.Display.setTextColor(TFT_GREENYELLOW, BLACK); 
      M5Cardputer.Display.print("> "); 
      M5Cardputer.Display.setTextColor(RED, BLACK); 
    } else {
      M5Cardputer.Display.setTextColor(BLACK, BLACK); 
      M5Cardputer.Display.print("  "); 
      M5Cardputer.Display.setTextColor(WHITE, BLACK); 
    }
     char formattedItem[maxItemLength + 1];
     snprintf(formattedItem, sizeof(formattedItem), "%-*s", maxItemLength, menuOptions[i]); 
     M5Cardputer.Display.println(formattedItem); 
  }
}

void executeAction(int index) {
  M5Cardputer.Display.clear(); 
  M5Cardputer.Display.setCursor(10,10); 
  M5Cardputer.Display.setTextColor(WHITE, BLACK); 
  M5Cardputer.Display.printf("Executing %s\n", menuOptions[index]); 
  delay(1000); 
  M5Cardputer.Display.clear(); 
  switch(index) {
  case 0: 
  break; 
  case 1:
  break; 
  case 2: 
  break; 
  case 3: 
  break; 
  case 4: 
  break; 
  case 5: 
  break; 
  default: 
  displayMenu(); 
  break; 
  }
  
}

void loop() {
  M5Cardputer.update(); 
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
    Keyboard_Class::KeysState key = M5Cardputer.Keyboard.keysState();
    for (int i : key.word) {
    if (i == ';') {
      menuIndex = (menuIndex > 0) ? menuIndex - 1 : menuItems -1; 
      displayMenu(); 
    } else if (i == ',') {
      menuIndex = (menuIndex < menuItems - 1) ? menuIndex + 1 : 0; 
      displayMenu(); 
    }  else if (i == '.') {
      menuIndex = (menuIndex < menuItems - 1) ? menuIndex + 1 : 0; 
      displayMenu(); 
    } else if (i == '1') {
      menuIndex = 0; 
      displayMenu(); 
    } else if (i == '2') {
      menuIndex = 1; 
      displayMenu();
    } else if (i == '3') {
      menuIndex = 2;
      displayMenu(); 
    } else if (i == '4') {
      menuIndex = 3; 
      displayMenu(); 
    } else if (i == '5') {
      menuIndex = 4; 
      displayMenu(); 
    } else if (i == '6') {
      menuIndex = 5; 
      displayMenu();
    } 
   }
   if (key.enter) {
     executeAction(menuIndex); 
   }
  }
  }
}
