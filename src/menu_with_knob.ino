#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>

ESP32Encoder encoder;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ENCODER_CLK 26
#define ENCODER_DT  25
#define ENCODER_SW  27
#define SENSOR_BEAKER 33 // Beaker sensor
#define SENSOR_FRESH 32 // Fresh water sensor
 // Motor pins
#define ENA 4
#define IN1 16
#define IN2 17

#define ENB 5
#define IN3 18
#define IN4 19


#define CH_LEFT  0
#define CH_RIGHT 1



long lastEncoderPos = 0;

enum ScreenState
{
  MENU,
  EDIT_BOTH,
  EDIT_LEFT,
  EDIT_RIGHT
};

ScreenState currentScreen = MENU;

int menuItem = 0;

int beakerLevel = 0; // Beaker output
int freshLevel = 0; // Fresh water input
int new_level = 0;

bool beakerOK = false;
bool freshOK = false;
bool wifiOK = false;

int lastCLK;

long last_input = 0;

const int pwmFreq = 5000;
const int pwmResolution = 8;

float levelToFlowLPerHr(int level)
{ 
    if (level <= 0)
    return 0;
  // return level * 2.43 / 100.0;
  return map(level ,0,100,16,243)/100.0;
}

int levelToPWM(int level)
{
  if (level <= 0)
    return 0;

  return map(level, 1, 100, 120, 255);
}

void drawBar(int x, int y, int w, int h, int percent)
{
  display.drawRect(x, y, w, h, SSD1306_WHITE);

  int fill = map(percent, 0, 100, 0, w - 2);

  display.fillRect(
    x + 1,
    y + 1,
    fill,
    h - 2,
    SSD1306_WHITE
  );
}

void drawMenu()
{
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("B:");
  display.print(beakerOK ? "O":"X");

  display.print(" F:");
  display.print(freshOK ? "O":"X");

  display.print(" W:");
  display.print(wifiOK ? "O":"X");

  display.setCursor(0,12);
  display.print("L:");
  display.print(levelToFlowLPerHr(beakerLevel), 1);
  display.print(" L/hr");

  display.setCursor(64,12);
  display.print("R:");
  display.print(levelToFlowLPerHr(freshLevel), 1);
  display.print(" L/hr");

  drawBar(0,26,50,8,beakerLevel);
  drawBar(74,26,50,8,freshLevel);

  const char* items[3] =
  {
    "Both",
    "Beaker",
    "Fresh"
  };

  for(int i=0;i<3;i++)
  {
    display.setCursor(10,38 + (i * 8));

    if(menuItem == i)
      display.print(">");

    else
      display.print(" ");

    display.print(items[i]);
  }
}

void drawEditor(const char *title, int value)
{
  Serial.println(title);
  display.setTextSize(1);

  display.setCursor(20,0);
  display.print(title);

  display.setCursor(20,16);
  display.print("Rate: ");
  display.print(levelToFlowLPerHr(value), 1);
  display.print(" L/hr");

  drawBar(14,32,100,12,value);

  display.setCursor(12,54);
  display.print("Click = Back");
}

void processEncoder()
{
    long currentPos = encoder.getCount();

    if(currentPos != lastEncoderPos)
    {
        long delta = currentPos - lastEncoderPos;
        if (delta > 0){
          last_input = millis();
        }
        switch(currentScreen)
        {
            case MENU:

                menuItem += delta;
                menuItem = constrain(menuItem, 0, 2);

                if(menuItem < 0)
                    menuItem = 2;

                if(menuItem > 2)
                    menuItem = 0;

                break;

            case EDIT_BOTH:

                beakerLevel += delta;
                freshLevel += delta;

                beakerLevel = constrain(beakerLevel, 0, 100);
                freshLevel = constrain(freshLevel, 0, 100);

                break;

            case EDIT_LEFT:

                beakerLevel += delta;
                beakerLevel = constrain(beakerLevel, 0, 100);

                break;

            case EDIT_RIGHT:

                freshLevel += delta;
                freshLevel = constrain(freshLevel, 0, 100);

                break;
        }

        lastEncoderPos = currentPos;
    }
    Serial.print("Menu=");
    Serial.print(menuItem);
    Serial.print(" Beaker=");
    Serial.print(beakerLevel);
    Serial.print(" Fresh=");
    Serial.println(freshLevel);
}

void processButton()
{
    static bool lastButton = HIGH;

    bool button = digitalRead(ENCODER_SW);

    if(lastButton == HIGH && button == LOW)
    {
        last_input = millis();

        if(currentScreen == MENU)
        {
            switch(menuItem)
            {
                case 0:
                    currentScreen = EDIT_BOTH;
                    break;

                case 1:
                    currentScreen = EDIT_LEFT;
                    break;

                case 2:
                    currentScreen = EDIT_RIGHT;
                    break;
            }
        }
        else
        {
            currentScreen = MENU;
        }

        delay(150);
    }

    lastButton = button;
}

void updateMotors()
{
    int freshPWM = 0;
    int beakerPWM = 0;

    if (freshOK)
    {
      if (beakerOK)
      {
        freshPWM = levelToPWM(freshLevel);
        beakerPWM = levelToPWM(beakerLevel);
      }
      else
      {
        freshPWM = 0;
        beakerPWM = levelToPWM(beakerLevel);
      }
    }

    ledcWrite(CH_LEFT, freshPWM);
    ledcWrite(CH_RIGHT, beakerPWM);
}


void setup()
{
  
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  encoder.attachHalfQuad(
      ENCODER_DT,
      ENCODER_CLK
  );

  encoder.setCount(0);

  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(SENSOR_BEAKER, INPUT);
  pinMode(SENSOR_FRESH, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  ledcSetup(CH_LEFT, 5000, 8);
  ledcSetup(CH_RIGHT, 5000, 8);

  ledcAttachPin(ENA, CH_LEFT);
  ledcAttachPin(ENB, CH_RIGHT);


  // Forward direction
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

}

void loop()
{
  beakerOK = !digitalRead(SENSOR_BEAKER);
  freshOK = digitalRead(SENSOR_FRESH);
  processEncoder();
  processButton();

  updateMotors();

  display.clearDisplay();
  if (!(millis() - last_input > 30000)){
    switch(currentScreen)
    {
    case MENU:
      drawMenu();
      break;

    case EDIT_BOTH:
      new_level = (beakerLevel + freshLevel)/2;
      beakerLevel = new_level;  
      freshLevel = new_level;
      drawEditor("SET BOTH",
                 (new_level));
      break;

    case EDIT_LEFT:
      drawEditor("SET BEAKER",
                 beakerLevel);
      break;

    case EDIT_RIGHT:
      drawEditor("SET FRESH",
                 freshLevel);
      break;
    }
  
  }

  display.display();
}