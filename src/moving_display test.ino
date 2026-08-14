// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// #define OLED_RESET -1
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// // Status flags
// bool beakerOK = false;    // X = Bad
// bool freshOK  = true;     // O = Good
// bool wifiOK   = true;     // O = Good

// float leftFlow  = 0.6;
// float rightFlow = 0.6;

// int leftLevel  = 50;  // %
// int rightLevel = 50;  // %

// int menuItem = 0;

// const char* menuItems[] = {
//   "Both",
//   "Left",
//   "Right"
// };

// void setup()
// {
//   Serial.begin(115200);

//   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
//   {
//     Serial.println("SSD1306 failed");
//     while (1);
//   }

//   display.clearDisplay();
//   display.setTextColor(SSD1306_WHITE);
// }

// void drawBar(int x, int y, int width, int height, int percent)
// {
//   display.drawRect(x, y, width, height, SSD1306_WHITE);

//   int fill = map(percent, 0, 100, 0, width - 2);

//   display.fillRect(
//     x + 1,
//     y + 1,
//     fill,
//     height - 2,
//     SSD1306_WHITE
//   );
// }

// void loop()
// {
//   display.clearDisplay();

//   // Header
//   display.setTextSize(1);
//   display.setCursor(0,0);

//   display.print("Beaker:");
//   display.print(beakerOK ? "O" : "X");

//   display.print(" H20:");
//   display.print(freshOK ? "O" : "X");

//   display.print(" Wifi:");
//   display.print(wifiOK ? "O" : "X");

//   // Flow rates
//   display.setCursor(0,16);
//   display.print("L:");
//   display.print(leftFlow,1);
//   display.print("L/h");

//   display.setCursor(64,16);
//   display.print("R:");
//   display.print(rightFlow,1);
//   display.print("L/h");

//   // Tank levels
//   leftLevel = leftLevel + 1;
//   if (leftLevel > 100) {
//   leftLevel = 0;
//   }

//   rightLevel =rightLevel + 1;
//   if (rightLevel > 100) {
//     rightLevel = 0;
//   }
//   display.setCursor(0,30);
//   display.print("Left");

//   display.setCursor(64,30);
//   display.print("Right");

//   drawBar(0,40,50,10,leftLevel);
//   drawBar(64,40,50,10,rightLevel);

//   // Menu
//   display.setCursor(0,55);

//   for(int i=0;i<3;i++)
//   {
//     if(i == menuItem)
//       display.print(">");

//     display.print(menuItems[i]);
//     display.print(" ");
//   }

//   display.display();

//   // Demo menu cycling
//   static unsigned long lastChange = 0;

//   if(millis() - lastChange > 2000)
//   {
//     menuItem++;
//     if(menuItem > 2)
//       menuItem = 0;

//     lastChange = millis();
//   }
// }