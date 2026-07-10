// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// #define SENSOR_PIN 32

// #define SENSOR_PIN2 33


// Adafruit_SSD1306 display(
//     SCREEN_WIDTH,
//     SCREEN_HEIGHT,
//     &Wire,
//     -1);

// void setup()
// {
//     Serial.begin(115200);

//     Wire.begin(21,22);

//     if(!display.begin(
//             SSD1306_SWITCHCAPVCC,
//             0x3C))
//     {
//         Serial.println("OLED FAIL");
//         while(true);
//     }

//     pinMode(SENSOR_PIN, INPUT);
//     pinMode(SENSOR_PIN2, INPUT);

//     display.clearDisplay();
//     display.display();

//     Serial.println("Started");
// }

// void loop()
// {
//     bool water = digitalRead(SENSOR_PIN);
//     bool water2 = digitalRead(SENSOR_PIN2);

//     display.clearDisplay();
//     display.setTextSize(2);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0,10);

//     if(water)
//     {
//         display.println("WATER");
//         Serial.println("WATER");
//     }
//     else
//     {
//         display.println("NO WATER");
//         Serial.println("NO WATER");
//     }

//     display.setCursor(0,30);

//     if(water2)
//     {
//         display.println("WATER");
//         Serial.println("WATER");
//     }
//     else
//     {
//         display.println("NO WATER");
//         Serial.println("NO WATER");
//     }

//     display.display();

//     delay(200);
// }