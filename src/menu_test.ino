// #include <ESP32Encoder.h>

// #define ENCODER_CLK 17
// #define ENCODER_DT  18
// #define ENCODER_SW  19

// ESP32Encoder encoder;

// long lastEncoderPos = 0;

// void processEncoder()
// {
//     long currentPos = encoder.getCount();

//     if (currentPos != lastEncoderPos)
//     {
//         long delta = currentPos - lastEncoderPos;

//         if (delta > 0)
//         {
//             Serial.print("CW  ");
//         }
//         else
//         {
//             Serial.print("CCW ");
//         }

//         Serial.print("Position: ");
//         Serial.println(currentPos);
//         Serial.print("Delta: ");
//         Serial.println(delta);
//         Serial.print("Last Position: ");
//         Serial.println(lastEncoderPos);
//         Serial.print("Current Position: ");
//         Serial.println(currentPos);
//         Serial.println("--------------------");
//         lastEncoderPos = currentPos;
//     }
// }

// void processButton()
// {
//     static bool lastButton = HIGH;

//     bool button = digitalRead(ENCODER_CLK);

//     if (lastButton == HIGH && button == LOW)
//     {
//         Serial.println("BUTTON PRESSED");
//         delay(150);
//     }

//     lastButton = button;
// }

// void setup()
// {
//     Serial.begin(115200);

//     pinMode(ENCODER_CLK, INPUT_PULLUP);

//     ESP32Encoder::useInternalWeakPullResistors = puType::up;

//     encoder.attachHalfQuad(
//         ENCODER_DT,
//         ENCODER_SW
//     );

//     encoder.setCount(0);

//     Serial.println();
//     Serial.println("Encoder Test Started");
//     Serial.println("Rotate knob and press button");
// }

// void loop()
// {
//     //     long newPosition = encoder.getCount();

//     //     if (newPosition != oldPosition)
//     //     {   
//     //         long delta = newPosition - oldPosition;
//     //         if (delta > 0){
//     //             Serial.print("CW  ");
//     //         } else {
//     //             Serial.print("CCW ");
//     //         }
//     //         // Serial.print(newPosition > oldPosition ? "CW  " : "CCW ");
//     //         Serial.print("Position: ");
//     //         Serial.println(newPosition);
//     //         oldPosition = newPosition;
//     //     }

//     //     static bool lastButton = HIGH;
//     //     //static bool stableButton = HIGH;
//     //     //static uint32_t lastDebounceTime = 0;

//     //     bool currentButton = digitalRead(ENCODER_SW);

//     //     if (lastButton == HIGH && currentButton == LOW)
//     //     {
//     //         Serial.println("BUTTON PRESSED");
//     //         // lastDebounceTime = millis();
//     //     }

//     //     // if ((millis() - lastDebounceTime) > 50)
//     //     // {
//     //     //     if (stableButton != currentButton)
//     //     //     {
//     //     //         stableButton = currentButton;
//     //     //         if (stableButton == LOW)
//     //     //         {
//     //     //             Serial.println("BUTTON PRESSED");
//     //     //         }
//     //     //     }
//     //     // }

//     //     lastButton = currentButton;
//     // }
//     processEncoder();
//     processButton();
// }