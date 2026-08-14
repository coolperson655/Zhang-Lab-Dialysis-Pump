//All this is done

// This file is the same as L298N_version.ino but with some changes.
// 1) the updateMotors() function now needs to use the library for the TB6612FNG motor driver instead of the L298N motor driver.
// 2) the warningScreen() function now displays a message to check the sensor or refill the tank when the fresh water is out.
// This warning screen should have a button to enable sensor override but automatically go back to the main screen once sensor readings are correct again
// 3) The addition of a setting menu, with a button to get to it at the bottom of the screen next to 'both', 'beaker', and 'fresh'
// clicking on the setting brings you to a new screen where you can toggle on/off wifi notifications, switch the motors from a max voltage of 5v or 12v(12 by default), 
// and a sensor override options that disables sensor logic.(override is off by default). The settings should be saved to the ESP32's flash memory so that they persist after a power cycle.
// 4) when handling encoder turning create and adaptive speed for turning. at low speeds it should be easy to make small adjustments, but at high speeds it should be easy to make large adjustments. 
//The speed should be based on the time between encoder turns, with a minimum of 1 and a maximum of 10. The speed should be applied to the menu item selection and the level adjustment. 
//The speed should be reset to 1 after a button press or after 2 seconds of no encoder activity.
//5) another option in the settings menu should be to calibrate the L/H reading.
// This will By running the pump for a set 'percent' for a known time(1 minute). This use will then enter the ammount that was pumped. This can be done for low medium and high percent 
// the rest will be extrapolated. The calibration data should be saved to the ESP32's flash memory so that it persists after a power cycle.
// For any code use exact copies from L2989N_verion.ino unless otherwise specified in the above list of changes.

//still need to do
// dont change encoder pins they are correct.
// warning anti burn in animation; the warning should not have the same sleep timer as the rest of the screnns so the text should be slowly animated get attention and prevent burn in.
// make sure timing functions will work over day long periods of time. millis() will overflow after 49 days, so use unsigned long and check for overflow when doing time calculations.
// speed up speed bar movement
// 5V mode- limits motor control to 5V maximum
// make wifi connection screen display connected or not conected when attempting
// make sensor booleans only activate when the sensor is in the correct state for 1 second to prevent false positives from small blips.
// add help section to the settings menu that explains how to use the machine and what the different settings do. This should be a scrollable text box that can be navigated with the encoder and button.
 
// on low 200mL in 13:50 4150 s/L 0.86 L/hr
// on high 200mL in 4:40 1400 s/L 2.54 L/hr
//1w idle. 4w lowest speed both motors 6w highest speed 3w idle came from wall plug led.

#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>
#include <Preferences.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define ENABLE_SMTP
#define ENABLE_DEBUG
#include <ReadyMail.h>

ESP32Encoder encoder;
Preferences preferences;

// Wifi credentials: replace with relevant ones
const char* ssid = "TMOBILE-FD74";
const char* password = "ESP32project";

// Sender SMTP settings (GMAIL)
// Change if using a different provider
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// Sender email, app password, and name
#define AUTHOR_EMAIL "esptempreciever655@gmail.com"
#define AUTHOR_APP_PASS "exyp rqzc lpcy pzbq"
#define AUTHOR_NAME "ESP32"

//Recipient's email
#define RECIPIENT_EMAIL "martikyl004@gmail.com"
#define RECIPIENT_NAME "Kyle"

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ENCODER_SW 17
#define ENCODER_DT 18
#define ENCODER_CLK 19
#define SENSOR_BEAKER 33
#define SENSOR_FRESH 34

#define PWMA 13
#define AIN1 14
#define AIN2 12
#define PWMB 25
#define BIN1 27
#define BIN2 26
// GPIO2 is connected to the ESP32's onboard blue LED on many boards, so use a different pin here.
#define STBY 4

#define CH_LEFT 0
#define CH_RIGHT 1

long lastEncoderPos = 0;
unsigned long lastEncoderActivity = 0;
float encoderSpeed = 1;

enum ScreenState {
  MENU,
  EDIT_BOTH,
  EDIT_LEFT,
  EDIT_RIGHT,
  SETTINGS,
  CALIBRATION,
  WARNING,
  MESSAGE_SENT,
  HELP
};
ScreenState pastScreen = MENU;
ScreenState currentScreen = MENU;
float menuCounter = 0.0;
int menuItem = 0;
float settingCounter = 0.0;
int settingsItem = 0;
int helpScroll = 0;
const char* helpText[] = {
  "Click to return to",
  "the settings menu.",
  "",
  "Turn the encoder to",
  "scroll this text.",
  "",
  "Use the encoder to",
  "choose settings.",
  "",
  "Press the button to",
  "select items.",
  "",
  "Wifi: toggle email",
  "alerts on/off.",
  "",
  "Override: bypass",
  "fresh-water sensor",
  "if you need to use",
  "the interface while",
  "refilling",
  "",
  "Help: view this",
  "information screen.",
  "",
  "To use,", 
  "place the beaker",
  "clip on the dialysis",
  "beaker. Make sure",
  "the sensors are on",
  "the beaker and the",
  "fresh water supply.",
  "",
  "The fresh water",
  "tube should be ",
  "Near the bottom",
  "of the beaker",
  "and the output",
  "tube near the",
  "dialysis tubes",
  "The fresh water",
  "sensor should",
  "have the light",
  "on.",
  "        ",
  "For regular use",
  "use the 'both'",
  "setting which sets",
  "the input pump",
  "slightly slower to",
  "prevent spilling."
  "           ",
  "The L/h values:",
  "are estimates,",
  "if percision values",
  "are needed measure",
  "them, yourself."
  "           ",
  "Additional info",
  "on the github",
  "'github link'"
  "",
  "",

};
const int helpLineCount = sizeof(helpText) / sizeof(helpText[0]);
float freshPumpSpeed = 0.0;
int calibrationSelection = 0;
int calibrationAmount = 0;
int calibrationStage = 0;
unsigned long encoderResetTime = 2000; // 2 seconds

int beakerLevel = 0;
int freshLevel = 0;
int new_level = 0;
int freshReductionLevel = 2; //reduces the fresh water level by 2% to prevent overfilling the beaker when both pumps are running. 
//This is a safety measure to ensure that the beaker does not overflow when both pumps are operating simultaneously.

bool beakerOK = false;
bool freshOK = false;
int badBeakerCounter = 0;  // tracks length of time the beaker sensor has been in the wrong state. If it is in the wrong state for too long, it will trigger a warning.
int badFreshCounter = 0; // same as above
int sensorDebounceCount = 5; // reduces the effect of small blips on the sensor readings. The sensor must be in the correct state for 5 consecutive checks to be considered valid.
bool wifiOK = true;
bool wifiConnected = false;
bool wifiConnecting = false;
int wifiCheck = 0;
int messageSendTime = 2000; // number of logic cycles before water email is sent. To ensure small blips done send false positives. 15 seconds
bool messageSent = false;
bool motorOK = true;
bool sensorOverride = false;
const unsigned long wifiConnectTimeoutMs = 10000;

bool motorVoltage12V = true;

int sleepTime = 120000; // 2 minutes - ms until screen turns off. This is to prevent burn in on the OLED display. The screen will turn back on when the encoder is turned or the button is pressed.

float lowCalibration = 1.0f;
float mediumCalibration = 1.0f;
float highCalibration = 1.0f;

bool showedWarning = false;
unsigned long last_input = 0;

const int pwmFreq = 5000;
const int pwmResolution = 8;

void drawWifiStartupScreen(const char* message, int spinnerIndex);
void drawMenu();
void drawEditor(const char* title, int value);
void drawSettings();
void drawHelp();
void drawCalibration();
void warningScreen();
void applySettings();
void loadSettings();
void resetEncoderSpeed();
void setMotorChannel(int channel, int pwm, bool forward);
void updateMotors();
void processEncoder();
void processButton();
void sendMessage(const char* message);
void processSensors();
bool wifiSetup();

float levelToFlowLPerHr(int level) {
  if (level <= 0) {
    return 0;
  }
  return map(level, 0, 100, 86, 254) / 100.0;
}

float calibratedFlowForLevel(int level) {
  float baseFlow = levelToFlowLPerHr(level);
  if (level < 33) {
    return baseFlow; //* lowCalibration;
  }
  if (level < 66) {
    return baseFlow;//* mediumCalibration;
  }
  return baseFlow; //* highCalibration;
}

int levelToPWM(int level) {
  if (level <= 0) {
    return 0;
  }
  return map(level, 1, 100, 120, 255);
}

void drawBar(int x, int y, int w, int h, int percent) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);
  int fill = map(percent, 0, 100, 0, w - 2);
  display.fillRect(x + 1, y + 1, fill, h - 2, SSD1306_WHITE);
}

void drawWifiStartupScreen(const char* message, int spinnerIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(8, 8);
  display.print("WiFi");
  display.setCursor(8, 24);
  display.print(message);
  display.setCursor(8, 40);
  display.print("Connecting");
  display.setCursor(88, 40);
  static const char spinnerChars[] = "|/-\\";
  display.print(spinnerChars[spinnerIndex % 4]);
  display.setCursor(8, 56);
  display.print("Boot will continue");
  display.display();
}

void drawMenu() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("B:");
  display.print(beakerOK ? "O" : "X");

  display.print(" F:");
  display.print(freshOK ? "O" : "X");

  display.print(" W:");
  display.print(wifiOK ? (wifiConnected ? "O" : (wifiConnecting ? "C" : "X")) : "OFF");

  display.print(" O:");
  display.print(sensorOverride ? "O" : "X");

  display.print(" V:");
  display.print(motorVoltage12V ? "12" : "5");

  display.setCursor(0, 12);
  display.print("F:");
  freshPumpSpeed = calibratedFlowForLevel(beakerLevel);
  display.print(freshPumpSpeed, 1);
  display.print(" L/hr");

  display.setCursor(64, 12);
  display.print("B:");
  display.print(calibratedFlowForLevel(freshLevel), 1);
  display.print(" L/hr");

  drawBar(0, 26, 50, 8, beakerLevel);
  drawBar(74, 26, 50, 8, freshLevel);

  const char* items[4] = {
    "Both",
    "Beaker",
    "Fresh",
    "Settings"
  };

  // Draw menu horizontally across the bottom to fit screen width
  int y = 44;
  int cols = 2;
  int colW = SCREEN_WIDTH / cols;  
  for (int i = 0; i < cols; i++) {
    int x = i * colW + 4;
    display.setCursor(x, y);
    if (menuItem == i) {
      display.print("[");
      display.print(items[i]);
      display.print("]");
    } else {
      display.print(" ");
      display.print(items[i]);
      display.print(" ");
    }
  }
    // Second row of menu
  y = 54;
  for (int i = 0; i < cols; i++) {
    int x = i * colW + 4;
    int j = i + cols; // Adjust index for second row
    display.setCursor(x, y);
    if (menuItem == j) {
        display.print("[");
        display.print(items[j]);
        display.print("]");
    } else {
        display.print(" ");
        display.print(items[j]);
        display.print(" ");
    }
  }
}

void drawEditor(const char* title, int value) {
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.print(title);

  display.setCursor(20, 16);
  display.print("Rate: ");
  display.print(calibratedFlowForLevel(value), 1);
  display.print(" L/hr");

  drawBar(14, 32, 100, 12, value);

  display.setCursor(12, 54);
  display.print("Click = Back");
}

void drawSettings() {
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("Settings");

  const char* items[5] = {
    "Wifi",
    "Motor",
    "Override",
    "Help",
    "Back"
  };

  for (int i = 0; i < 5; i++) {
    display.setCursor(6, 12 + (i * 10));
    if (settingsItem == i) {
      display.print(">");
    } else {
      display.print(" ");
    }

    display.print(items[i]);

    if (i == 0) {
      display.print(": ");
      display.print(wifiOK ? "ON" : "OFF");
    } else if (i == 1) {
      display.print(": ");
      display.print(motorVoltage12V ? "12V" : "5V");
    } else if (i == 2) {
      display.print(": ");
      display.print(sensorOverride ? "ON" : "OFF");
    }
  }
}

void drawHelp() {
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("Help");

  int visibleLines = 5;
  int startLine = helpScroll;
  int endLine = min(startLine + visibleLines, helpLineCount);

  for (int i = startLine; i < endLine; i++) {
    display.setCursor(6, 18 + ((i - startLine) * 10));
    display.print(helpText[i]);
  }


}

void drawCalibration() {
  display.setTextSize(1);
  display.setCursor(8, 0);
  display.print("Calibrate L/H");
  display.setCursor(8, 14);
  display.print("Stage:");
  if (calibrationStage == 0) {
    display.print("Low");
  } else if (calibrationStage == 1) {
    display.print("Medium");
  } else {
    display.print("High");
  }

  display.setCursor(8, 28);
  display.print("Percent:");
  display.print(calibrationSelection);
  display.print("%");

  display.setCursor(8, 42);
  display.print("Amount mL:");
  display.print(calibrationAmount);

  display.setCursor(8, 56);
  display.print("Click = Save");
}

void warningScreen() {
    // need to add anti burn in animation to this text
  if (!showedWarning) {
    pastScreen = currentScreen;
    currentScreen = WARNING;
    showedWarning = true;
    motorOK = false;
    digitalWrite(STBY, LOW);
    if (wifiOK) {
        wifiCheck = 0;
    } 
  }
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (!messageSent) {
    display.print("WARNING, email:" + ((wifiConnected && wifiOK) ? String((messageSendTime-wifiCheck)/33) : "None"));
    display.setCursor(0, 16);
    display.print("Fresh water is out.");
    display.setCursor(0, 32);
    display.print("Check sensor or       refill tank.");
    display.setCursor(0, 48);
    display.print("Press button to      override");

    if (wifiConnected && wifiOK) { 
        wifiCheck += 1;
        if ((wifiCheck > messageSendTime) & !messageSent) {
            String msgString = "Fresh water in Pump Machine 1 is empty after running for " + String(millis() / 3600000) + " hours( " +  String(millis() / 1000) + "seconds), at " + String(freshPumpSpeed, 1) + " L/h. Beaker is " + (beakerOK ? String("Good") : String("Bad"));
            sendMessage(msgString.c_str());
            messageSent = true;
            wifiCheck = 0;
        }
    }
    // add message send timer
  } else {
    display.print("WARNING MESSAGE SENT");
    display.setCursor(0, 16);
    display.print("Fresh water is out.");
    display.setCursor(0, 32);
    display.print("Press button to      reset. ");
  }
  processButton();
}

void applySettings() {
  preferences.begin("pumpcfg", false);
  preferences.putBool("wifi", wifiOK);
  preferences.putBool("motor12v", motorVoltage12V);
  preferences.putBool("sensorOverride", sensorOverride);
  preferences.putFloat("calLow", lowCalibration);
  preferences.putFloat("calMed", mediumCalibration);
  preferences.putFloat("calHigh", highCalibration);
  preferences.end();
}

void loadSettings() {
  preferences.begin("pumpcfg", true);
  wifiOK = preferences.getBool("wifi", true);
  motorVoltage12V = preferences.getBool("motor12v", true);
  sensorOverride = preferences.getBool("sensorOverride", false);
  lowCalibration = preferences.getFloat("calLow", 1.0f);
  mediumCalibration = preferences.getFloat("calMed", 1.0f);
  highCalibration = preferences.getFloat("calHigh", 1.0f);
  preferences.end();
}

void resetEncoderSpeed() {
  encoderSpeed = 0.1;
  lastEncoderActivity = millis();
}

void setMotorChannel(int channel, int pwm, bool forward) {
  if (channel == CH_LEFT) {
    analogWrite(PWMA, pwm);
    digitalWrite(AIN1, forward ? HIGH : LOW);
    digitalWrite(AIN2, forward ? LOW : HIGH);
  } else {
    analogWrite(PWMB, pwm);
    digitalWrite(BIN1, forward ? HIGH : LOW);
    digitalWrite(BIN2, forward ? LOW : HIGH);
  }
}

void updateMotors() {
  int freshPWM = 0;
  int beakerPWM = 0;

  bool allowMotors = motorOK && (freshOK || sensorOverride);

  if (allowMotors) {
    digitalWrite(STBY, HIGH);
    if (beakerOK) {
      int freshOutputLevel = constrain(freshLevel - freshReductionLevel, 0, 100);
      freshPWM = levelToPWM(freshOutputLevel);
      beakerPWM = levelToPWM(beakerLevel);
    } else {
      freshPWM = 0;
      beakerPWM = levelToPWM(beakerLevel);
    }
  } else {
    digitalWrite(STBY, LOW);
  }

  setMotorChannel(CH_LEFT, freshPWM, true);
  setMotorChannel(CH_RIGHT, beakerPWM, true);
}

void processEncoder() {
  long currentPos = encoder.getCount();
  unsigned long now = millis();

  if (currentPos != lastEncoderPos) {
    long delta = currentPos - lastEncoderPos;
    lastEncoderActivity = millis();
    if (delta != 0) {
        int movement = delta > 0 ? -1 : 1;
    //   unsigned long gap = now - lastEncoderActivity;
    //   int direction = (delta > 0) ? 1 : -1;

    //   if (gap >= encoderResetTime) {
    //     encoderSpeed = 1.0f;
    //   } else {
    //     float speedFromGap = map((long)gap, 0, (long)encoderResetTime, 10.0f, 1.0f);
    //     encoderSpeed = constrain(speedFromGap, 1.0f, 10.0f);
    //   }

    //   int step = constrain((int)round(encoderSpeed), 1, 10);
    //   int movement = step * direction;
    //   int menuMovement = constrain((int)round(menuCounter * direction), 0, 1);
    //   menuCounter += (float)movement / 10.0f; // Accumulate fractional

    //   lastEncoderActivity = now;

    //   Serial.print("Movement: ");
    //   Serial.println(menuCounter);
      switch (currentScreen) {
        case MENU:
          menuCounter += (float)movement / 2.0f; // Accumulate fractional
          menuCounter = constrain(menuCounter, 0.0f, 3.0f); // Constrain to menu item range
          menuItem = (int)round(menuCounter);
        //   Serial.print("item: ");
        //   Serial.println(menuItem);
          //menuItem = constrain(menuItem + menuMovement, 0, 3);
          break;
        case EDIT_BOTH:
          beakerLevel = constrain(beakerLevel + movement, 0, 100);
          freshLevel = constrain(freshLevel + movement, 0, 100);
          break;
        case EDIT_LEFT:
          beakerLevel = constrain(beakerLevel + movement, 0, 100);
          break;
        case EDIT_RIGHT:
          freshLevel = constrain(freshLevel + movement, 0, 100);
          break;
        case SETTINGS:
          settingCounter += (float)movement / 2.0f; // Accumulate fractional
          settingCounter = constrain(settingCounter, 0.0f, 4.0f); // Constrain to settings item range
          settingsItem = (int)round(settingCounter);
          break;
        case HELP:
          helpScroll = constrain(helpScroll + movement, 0, max(0, helpLineCount - 5));
          break;
        case CALIBRATION:
          calibrationSelection = constrain(calibrationSelection + movement, 10, 100);
          calibrationAmount = constrain(calibrationAmount + movement * 10, 0, 1000);
          break;
      }
    }
    lastEncoderPos = currentPos;
  } else if ((now - lastEncoderActivity) >= encoderResetTime) {
    encoderSpeed = 1.0f;
  }
}

void processButton() {
  static bool lastButton = HIGH;
  // Read the encoder switch (button) pin
  bool button = digitalRead(ENCODER_SW);

  if (lastButton == HIGH && button == LOW) {
    lastEncoderActivity = millis();
    resetEncoderSpeed();

    if (currentScreen == MENU) {
      switch (menuItem) {
        case 0:
          currentScreen = EDIT_BOTH;
          break;
        case 1:
          currentScreen = EDIT_LEFT;
          break;
        case 2:
          currentScreen = EDIT_RIGHT;
          break;
        case 3:
          currentScreen = SETTINGS;
          break;
      }
    } else if (currentScreen == SETTINGS) {
      if (settingsItem == 3) {
        helpScroll = 0;
        currentScreen = HELP;
        // calibrationStage = 0;
        // calibrationSelection = 25;
        // calibrationAmount = 50;
        // currentScreen = CALIBRATION;
      } else {
        if (settingsItem == 0) {
          wifiOK = !wifiOK;
          applySettings();
          if (wifiOK) {
            wifiSetup();
          } else {
            wifiConnected = false;
            wifiConnecting = false;
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
          }
        } else if (settingsItem == 1) {
          motorVoltage12V = !motorVoltage12V;
        } else if (settingsItem == 2) {
          sensorOverride = !sensorOverride;
        } else if (settingsItem == 4) {
          // Handle "Back" option
          currentScreen = MENU;
        }
        applySettings();
      }
    } else if (currentScreen == CALIBRATION) {
      if (calibrationStage == 0) {
        lowCalibration = (float)calibrationAmount / max(1, calibrationSelection);
      } else if (calibrationStage == 1) {
        mediumCalibration = (float)calibrationAmount / max(1, calibrationSelection);
      } else {
        highCalibration = (float)calibrationAmount / max(1, calibrationSelection);
      }
      calibrationStage = (calibrationStage + 1) % 3;
      calibrationSelection = 25;
      calibrationAmount = 50;
      applySettings();
      currentScreen = SETTINGS;
    } else if (currentScreen == WARNING) {
      // Toggle sensor override from the warning screen (click to override)
        if (!messageSent) {
            sensorOverride = !sensorOverride;
            applySettings();
        } else {
            messageSent = !messageSent;
        }
    } else if (currentScreen == HELP) {
      currentScreen = SETTINGS;
    } else {
      currentScreen = MENU;
    }

    delay(150);
  }

  lastButton = button;
}

void processSensors() {
  beakerOK = !digitalRead(SENSOR_BEAKER);
  freshOK = digitalRead(SENSOR_FRESH);
  if (!beakerOK) {
    badBeakerCounter += 1;
  } else {
    badBeakerCounter = 0;
  }
  if (!freshOK) {
    badFreshCounter += 1;
  } else {
    badFreshCounter = 0;
  }
  if (badBeakerCounter > sensorDebounceCount) {
    beakerOK = false;
  } else {
    beakerOK = true;
  }
  if (badFreshCounter > sensorDebounceCount) {
    freshOK = false;
  } else {
    freshOK = true;
  }
}
void sendMessage(const char* message) {
  if (smtp.isConnected()) {
    smtp.authenticate(AUTHOR_EMAIL, AUTHOR_APP_PASS, readymail_auth_password);

    SMTPMessage msg;

    msg.headers.add(rfc822_from, String(AUTHOR_NAME) + " <" + AUTHOR_EMAIL + ">");
    msg.headers.add(rfc822_to, String(RECIPIENT_NAME) + " <" + RECIPIENT_EMAIL + ">");
    msg.headers.add(rfc822_subject, "Dialysis update: Machine 1");
    msg.text.body(message);
    //msg.html.body("<html><body><h1>Hello!</h1></body></html>");
     
    // Set NTP config time
    /* For times east of the Prime Meridian use 0-12
    For times west of the Prime Meridian add 12 to the offset.
    Ex. American/Denver GMT would be -6. 6 + 12 = 18 */
    const int gmtOffset_sec = 18; //offset time in seconds
    const int daylightOffset_sec = 0; //daylight saving time offset in seconds

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
    // Set timestamp for the email
    while (time(nullptr) < 100000) delay(100);
    msg.timestamp = time(nullptr);

    smtp.send(msg);
  }
}

bool wifiSetup() {
  if (!wifiOK) {
    wifiConnected = false;
    wifiConnecting = false;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return false;
  }

  wifiConnecting = true;
  wifiConnected = false;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  int spinnerIndex = 0;
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < wifiConnectTimeoutMs) {
    spinnerIndex++;
    drawWifiStartupScreen("Trying to connect", spinnerIndex);
    delay(200);
  }

  wifiConnecting = false;
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    ssl_client.setInsecure();

    auto statusCallback = [](SMTPStatus status) {
      Serial.println(status.text);
    };

    smtp.connect(SMTP_HOST, SMTP_PORT, statusCallback);
    drawWifiStartupScreen("WiFi Connected", spinnerIndex);
    delay(1000);
    return true;
  }

  drawWifiStartupScreen("Failed to connect", spinnerIndex);
  delay(1000);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiConnected = false;
  return false;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();

  loadSettings();
  applySettings();

  // gmail connection
  wifiSetup();
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // Attach encoder DT = A, CLK = B (swap if your wiring differs)
  encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
  encoder.setCount(0);

  // Configure encoder switch and clk inputs
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(SENSOR_BEAKER, INPUT);
  pinMode(SENSOR_FRESH, INPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);
  delay(50);
  digitalWrite(STBY, HIGH);

  ledcSetup(CH_LEFT, pwmFreq, pwmResolution);
  ledcSetup(CH_RIGHT, pwmFreq, pwmResolution);
  ledcAttachPin(PWMA, CH_LEFT);
  ledcAttachPin(PWMB, CH_RIGHT);

  lastEncoderActivity = millis();
}

void loop() {
  processSensors();
  // If fresh water is out and override is not enabled, show warning and
  // freeze normal controls. Allow the button to toggle override from
  // the warning screen.
  display.clearDisplay();
//   Serial.print("Fresh OK: ");
//   Serial.println(freshOK);
//   Serial.print("Sensor Override: ");
//   Serial.println(sensorOverride);

  if (!freshOK && !sensorOverride) {
    // Show warning and allow only button to toggle override
    warningScreen();
    //Serial.println("In warning screen, waiting for button press to override.");
  } else {
    // Normal operation: process encoder/button and update motors
    processEncoder();
    processButton();
    updateMotors();
    //Serial.println("Normal operation, processing encoder and button.");

    if ((millis() - lastEncoderActivity) <= sleepTime)  {//} || currentScreen != MENU) {
      //Serial.println("loop");
      if (showedWarning) {
        showedWarning = false;
        currentScreen = pastScreen;
        motorOK = true;
        wifiCheck = 0;
      }
        // Keep the current screen active
      // Serial.println("Override: " + String(sensorOverride));
      switch (currentScreen) {
        case MENU:
          drawMenu();
          break;
        case EDIT_BOTH:
          new_level = (beakerLevel + freshLevel) / 2;
          beakerLevel = new_level;
          freshLevel = new_level;
          drawEditor("SET BOTH", new_level);
          break;
        case EDIT_LEFT:
          drawEditor("SET BEAKER", beakerLevel);
          break;
        case EDIT_RIGHT:
          drawEditor("SET FRESH", freshLevel);
          break;
        case SETTINGS:
          drawSettings();
          break;
        case HELP:
          drawHelp();
          break;
        case CALIBRATION:
          drawCalibration();
          break;
      }
    }
  }
  display.display();
}