// #include <ESP32Encoder.h>

// #define ENCODER_CLK 26
// #define ENCODER_DT  25
// #define ENCODER_SW  27

// ESP32Encoder encoder;

// long oldPosition = 0;

// void setup()
// {
//     Serial.begin(115200);

//     pinMode(ENCODER_SW, INPUT_PULLUP);

//     // Enable weak pullups
//     ESP32Encoder::useInternalWeakPullResistors =
//         puType::up;

//     encoder.attachHalfQuad(
//         ENCODER_DT,
//         ENCODER_CLK
//     );

//     encoder.setCount(0);

//     Serial.println();
//     Serial.println("Encoder Test Started");
//     Serial.println("Rotate knob and press button");
// }

// void loop()
// {
//     long newPosition = encoder.getCount();

//     if(newPosition != oldPosition)
//     {
//         if(newPosition > oldPosition)
//         {
//             Serial.print("CW  ");
//         }
//         else
//         {
//             Serial.print("CCW ");
//         }

//         Serial.print("Position: ");
//         Serial.println(newPosition);

//         oldPosition = newPosition;
//     }

//     static bool lastButton = HIGH;
//     bool currentButton = digitalRead(ENCODER_SW);

//     if(lastButton == HIGH && currentButton == LOW)
//     {
//         Serial.println("BUTTON PRESSED");
//         delay(150);
//     }

//     lastButton = currentButton;
// }