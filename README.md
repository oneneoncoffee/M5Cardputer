# M5Cardputer
Arduino IDE 2.3.3 Tested and working with library's installed. 
List Inlcudes: 
1. #include <M5Cardputer.h>
2. #include <M5Unified.h>
3. #include "M5GFX.h" 
4. #include <IRremoteESP8266.h>
5. #include <IRsend.h>
6. #include <IRutils.h>
7. #include "WiFi.h"
8. #include <esp_wifi.h>
9. #include <WebServer.h>
10. #include "database.h"  //database.h <-- This one is custom to our sketch
11. #include <time.h>
12. #include <SD.h>
13. #include "USB.h"
14. #include "USBHIDKeyboard.h"

whitcat.ino - M5Cardputer Source code project. Arduino open source code public version 1  bloated and buggy but functional. 
// What can it do ? 
1. Essid spam
2. Basic battery percentage. 
3. Tv-B-gone 
4. Dice game
5. Pixel starfield 
6. Keyboard HID emulation
7. Bad USB HID emulation
8. Headless server that scans networks.   

M5Cardputer_MENU.ino - M5Cardputer Arduino menu system. working example. 

WhiteCatV2.ino - Version 2.0 of white Cat. 2nd draft less buggy but bloated will rewrite the source code.
                 database.h just includes IR codes and xbitmap arrays. 
database.h - IR codes and xbitmap arrays only. So you have to include this in WhiteCatV2.ino sketch for it to function. 
// What can it do ? 
1. Tv-B-gone / IR TECH BLASTER
2. Basic battery percentage/Voltage avg/Overall battery 
3. Funny names Essid sapm
4. Rnadom Name essid spam
5. Random Packet anonymous essid spam.
6. Headless server that scans networks
7. Headless server you can rename.
8. Clones scanned essids (EVIL TWIN) like outline only.
9. Keyboard HID with input KEYLOGGER
10. SD Card support essid spam lists
11. Dice game / D20 dice game.
12. Pixel starfield

How our are sketches buggy? 
1st issue the battery precent is really weird because its showing current of the battery. 
2nd issue xbitmaps are black and white not color and the image data can't resized. Bloats the overall sketch size. 
3rd Bad code base and conflecting librarys. 

