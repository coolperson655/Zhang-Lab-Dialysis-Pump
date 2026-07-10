
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Encoder.h>

// #define ENC_A 2
// #define ENC_B 4
// #define ENC_SW 15



// #define SCREEN_WIDTH 128 // OLED display width, in pixels

// #define SCREEN_HEIGHT 64 // OLED display height, in pixels


// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// void setup() {

//   Serial.begin(115200);
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.clearDisplay();

 

//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.setCursor(10, 20);
//   display.print("Hello World");
//   display.display();

//   Serial.println('Testprint');

//   pinMode(ENC_A, INPUT_PULLUP);
//   pinMode(ENC_B, INPUT_PULLUP);
//   pinMode(ENC_SW, INPUT_PULLUP);


// }


// void loop() {
  
//   static int lastA = digitalRead(ENC_A);

//   int a = digitalRead(ENC_A);

//   if (a != lastA) {

//     if (digitalRead(ENC_B) != a)
//       Serial.println("CW");
//     else
//       Serial.println("CCW");

//     lastA = a;
//   }

//   if (!digitalRead(ENC_SW))
//     Serial.println("BUTTON");

//   delay(1);

//   // Nothing here as we only need to display text once

// }

// // #define SCREEN_WIDTH 128
// // #define SCREEN_HEIGHT 64

// // Adafruit_SSD1306 display(
// //   SCREEN_WIDTH,
// //   SCREEN_HEIGHT,
// //   &Wire,
// //   -1
// // );

// // // Encoder
// // Encoder encoder(32, 33);
// // const int buttonPin = 35;

// // // L298N Pins
// // const int ENA = 26;
// // const int IN1 = 13;
// // const int IN2 = 12;

// // const int ENB = 25;
// // const int IN3 = 14;
// // const int IN4 = 27;

// // // PWM Settings
// // const int PWM_FREQ = 20000;
// // const int PWM_RES = 8;

// // // PWM Channels
// // const int CH_A = 0;
// // const int CH_B = 1;

// // // Menu
// // enum MenuItems {
// //   MOTORA_SPEED,
// //   MOTORA_DIR,
// //   MOTORB_SPEED,
// //   MOTORB_DIR
// // };

// // int menuIndex = 0;
// // bool editMode = false;

// // int motorASpeed = 128;
// // int motorBSpeed = 128;

// // int motorADir = 1;  // 1=fwd, -1=rev, 0=stop
// // int motorBDir = 1;

// // long oldPosition = 0;
// // bool lastButtonState = HIGH;

// // void drawMenu() {

// //   display.clearDisplay();
// //   display.setTextSize(1);
// //   display.setTextColor(SSD1306_WHITE);

// //   display.setCursor(0,0);
// //   display.println("L298N Controller");

// //   const char* marker;

// //   marker =
// //       (menuIndex == MOTORA_SPEED)
// //       ? ">"
// //       : " ";

// //   display.printf(
// //       "%sA Speed: %3d\n",
// //       marker,
// //       motorASpeed);

// //   marker =
// //       (menuIndex == MOTORA_DIR)
// //       ? ">"
// //       : " ";

// //   display.printf(
// //       "%sA Dir: %s\n",
// //       marker,
// //       String(motorADir));

// //   marker =
// //       (menuIndex == MOTORB_SPEED)
// //       ? ">"
// //       : " ";

// //   display.printf(
// //       "%sB Speed: %3d\n",
// //       marker,
// //       motorBSpeed);

// //   marker =
// //       (menuIndex == MOTORB_DIR)
// //       ? ">"
// //       : " ";

// //   display.printf(
// //       "%sB Dir: %s\n",
// //       marker,
// //       String(motorBDir));

// //   display.setCursor(0,56);

// //   if(editMode)
// //     display.print("EDIT");
// //   else
// //     display.print("NAV");

// //   display.display();
// // }

// // void handleEncoder() {

// //   long position = encoder.read() / 4;

// //   if(position != oldPosition) {

// //     int delta = position - oldPosition;
// //     oldPosition = position;

// //     if(!editMode) {

// //       menuIndex += delta;

// //       if(menuIndex < 0)
// //         menuIndex = 3;

// //       if(menuIndex > 3)
// //         menuIndex = 0;
// //     }
// //     else {

// //       switch(menuIndex) {

// //         case MOTORA_SPEED:
// //           motorASpeed += delta * 5;
// //           motorASpeed =
// //               constrain(motorASpeed,0,255);
// //           break;

// //         case MOTORB_SPEED:
// //           motorBSpeed += delta * 5;
// //           motorBSpeed =
// //               constrain(motorBSpeed,0,255);
// //           break;

// //         case MOTORA_DIR:
// //           motorADir += delta;

// //           if(motorADir > 1)
// //             motorADir = -1;

// //           if(motorADir < -1)
// //             motorADir = 1;
// //           break;

// //         case MOTORB_DIR:
// //           motorBDir += delta;

// //           if(motorBDir > 1)
// //             motorBDir = -1;

// //           if(motorBDir < -1)
// //             motorBDir = 1;
// //           break;
// //       }
// //     }

// //     drawMenu();
// //   }
// // }

// // void handleButton() {

// //   bool current =
// //       digitalRead(buttonPin);

// //   if(lastButtonState == HIGH &&
// //      current == LOW)
// //   {
// //     editMode = !editMode;
// //     drawMenu();
// //     delay(200);
// //   }

// //   lastButtonState = current;
// // }

// // const char* String(int d) {

// //   if(d == 1)
// //     return "FWD";

// //   if(d == -1)
// //     return "REV";

// //   return "STOP";
// // }

// // void updateMotors() {

// //   // Motor A
// //   if(motorADir == 1) {

// //     digitalWrite(IN1,HIGH);
// //     digitalWrite(IN2,LOW);

// //     ledcWrite(CH_A,motorASpeed);
// //   }
// //   else if(motorADir == -1) {

// //     digitalWrite(IN1,LOW);
// //     digitalWrite(IN2,HIGH);

// //     ledcWrite(CH_A,motorASpeed);
// //   }
// //   else {

// //     digitalWrite(IN1,LOW);
// //     digitalWrite(IN2,LOW);

// //     ledcWrite(CH_A,0);
// //   }

// //   // Motor B
// //   if(motorBDir == 1) {

// //     digitalWrite(IN3,HIGH);
// //     digitalWrite(IN4,LOW);

// //     ledcWrite(CH_B,motorBSpeed);
// //   }
// //   else if(motorBDir == -1) {

// //     digitalWrite(IN3,LOW);
// //     digitalWrite(IN4,HIGH);

// //     ledcWrite(CH_B,motorBSpeed);
// //   }
// //   else {

// //     digitalWrite(IN3,LOW);
// //     digitalWrite(IN4,LOW);

// //     ledcWrite(CH_B,0);
// //   }
// // }



// // void setup() {

// //   Serial.begin(115200);

// //   pinMode(buttonPin, INPUT_PULLUP);

// //   pinMode(IN1, OUTPUT);
// //   pinMode(IN2, OUTPUT);

// //   pinMode(IN3, OUTPUT);
// //   pinMode(IN4, OUTPUT);

// // ledcAttachPin(ENA, PWM_FREQ, PWM_RES);

// // ledcAttach(ENB, PWM_FREQ, PWM_RES);

// //   if(!display.begin(
// //       SSD1306_SWITCHCAPVCC,
// //       0x3C))
// //   {
// //     while(true);
// //   }

// //   display.clearDisplay();
// //   display.display();

// //   updateMotors();
// //   drawMenu();
// // }

// // void loop() {

// //   handleEncoder();
// //   handleButton();

// //   updateMotors();
// // }

