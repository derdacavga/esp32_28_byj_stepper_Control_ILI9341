#include <TFT_eSPI.h>
#include <AccelStepper.h>

const int motorPin1 = 18;
const int motorPin2 = 8;
const int motorPin3 = 16;
const int motorPin4 = 17;

AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

TFT_eSPI tft = TFT_eSPI();

volatile bool motorRunning = false;
volatile bool motorDir = true;
volatile long stepCounter = 0;
volatile long turnCounter = 0;
const long stepsPerTurn = 2048;
long lastStepCounter = -1;
long lastTurnCounter = -1;
volatile int displayedSpeed = 50;

const int startStopButtonX = 50;
const int startStopButtonY = 260;
const int startStopButtonW = 140;
const int startStopButtonH = 40;

const int cwButtonX = 20;
const int cwButtonY = 150;
const int dirButtonW = 90;
const int dirButtonH = 30;

const int ccwButtonX = 130;
const int ccwButtonY = 150;

const int speedUpButtonX = 140;
const int speedUpButtonY = 200;
const int speedButtonW = 60;
const int speedButtonH = 40;

const int speedDownButtonX = 40;
const int speedDownButtonY = 200;

const int maxMotorSpeed = 700;
const int minMotorSpeed = 10;

const unsigned long DEBOUNCE_DELAY = 90;
unsigned long lastTouchTime = 0;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(2);
  logo();
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  tft.calibrateTouch(nullptr, TFT_MAGENTA, TFT_BLACK, 15);

  stepper.setMaxSpeed(maxMotorSpeed);
  stepper.setAcceleration(50);

  drawUI();
  updateCounterDisplay();
  drawSpeedValue();

  xTaskCreatePinnedToCore(
    motorTask,    // Task Func
    "MotorTask",  // Task Name
    4096,         // Heap Size
    NULL,         // Parameter
    1,            // priority
    NULL,         // Task Handle
    0             // Core
  );

  xTaskCreatePinnedToCore(
    uiTask,    // Task Func
    "UITask",  // Task Name
    4096,      // Heap Size
    NULL,      // Parameter
    1,         // Priority
    NULL,      // Task Handle
    1          // Core
  );
}

void motorTask(void *parameter) {
  for (;;) {
    if (motorRunning) {
      float targetSpeed = map(displayedSpeed, 0, 100, minMotorSpeed, maxMotorSpeed);
      stepper.setSpeed(motorDir ? -targetSpeed : targetSpeed);
      stepper.runSpeed();
      long currentStep = stepper.currentPosition();
      turnCounter = currentStep / stepsPerTurn;
      stepCounter = abs(currentStep % stepsPerTurn);
    } else {
      stepper.setSpeed(0);
    }
    vTaskDelay(1);
  }
}

void uiTask(void *parameter) {
  for (;;) {
    uint16_t x, y;
    unsigned long currentTime = millis();

    if (tft.getTouch(&x, &y) && (currentTime - lastTouchTime >= DEBOUNCE_DELAY)) {
      lastTouchTime = currentTime;

      if (inArea(x, y, startStopButtonX, startStopButtonY, startStopButtonW, startStopButtonH)) {
        motorRunning = !motorRunning;
        drawStartStopButton();
      }

      if (inArea(x, y, cwButtonX, cwButtonY, dirButtonW, dirButtonH)) {
        motorDir = true;
        drawDirectionButtons();
      }

      if (inArea(x, y, ccwButtonX, ccwButtonY, dirButtonW, dirButtonH)) {
        motorDir = false;
        drawDirectionButtons();
      }

      if (inArea(x, y, speedUpButtonX, speedUpButtonY, speedButtonW, speedButtonH)) {
        if (displayedSpeed < 100) {
          displayedSpeed += 10;
          updateMotorSpeed();
          drawSpeedValue();
        }
      }

      if (inArea(x, y, speedDownButtonX, speedDownButtonY, speedButtonW, speedButtonH)) {
        if (displayedSpeed > 0) {
          displayedSpeed -= 10;
          updateMotorSpeed();
          drawSpeedValue();
        }
      }
    }

    updateCounterDisplay();

    vTaskDelay(10);
  }
}

void loop() {
}

void updateMotorSpeed() {
  float targetSpeed = map(displayedSpeed, 0, 100, minMotorSpeed, maxMotorSpeed);
  stepper.setMaxSpeed(targetSpeed);
}

bool inArea(uint16_t tx, uint16_t ty, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  return (tx >= x && tx <= (x + w) && ty >= y && ty <= (y + h));
}

void updateCounterDisplay() {
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_24);
  if (turnCounter != lastTurnCounter) {
    tft.fillRect(100, 10, 160, 30, TFT_BLACK);
    tft.drawString(String(turnCounter), 100, 10);
    lastTurnCounter = turnCounter;
  }
  if (stepCounter != lastStepCounter) {
    tft.fillRect(100, 40, 160, 30, TFT_BLACK);
    tft.drawString(String(stepCounter), 100, 40);
    lastStepCounter = stepCounter;
  }
}

void drawStartStopButton() {
  tft.fillRoundRect(startStopButtonX, startStopButtonY, startStopButtonW, startStopButtonH, 10, motorRunning ? TFT_RED : TFT_GREEN);
  tft.drawCentreString(motorRunning ? "STOP" : "START", startStopButtonX + startStopButtonW / 2, startStopButtonY + startStopButtonH / 2 - 12, 2);
}

void drawDirectionButtons() {
  tft.fillRoundRect(cwButtonX, cwButtonY, dirButtonW, dirButtonH, 10, motorDir ? TFT_GREEN : TFT_DARKGREY);
  tft.drawCentreString("CW", cwButtonX + dirButtonW / 2, cwButtonY + dirButtonH / 2 - 12, 2);
  tft.fillRoundRect(ccwButtonX, ccwButtonY, dirButtonW, dirButtonH, 10, !motorDir ? TFT_GREEN : TFT_DARKGREY);
  tft.drawCentreString("CCW", ccwButtonX + dirButtonW / 2, ccwButtonY + dirButtonH / 2 - 12, 2);
}

void drawSpeedButtons() {
  tft.fillRoundRect(speedUpButtonX, speedUpButtonY, speedButtonW, speedButtonH, 8, TFT_DARKGREY);
  tft.drawCentreString("+", speedUpButtonX + speedButtonW / 2, speedUpButtonY + speedButtonH / 2 - 12, 2);
  tft.fillRoundRect(speedDownButtonX, speedDownButtonY, speedButtonW, speedButtonH, 8, TFT_DARKGREY);
  tft.drawCentreString("-", speedDownButtonX + speedButtonW / 2, speedDownButtonY + speedButtonH / 2 - 12, 2);
}

void drawSpeedValue() {
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.fillRect(120, 80, 140, 30, TFT_BLACK);
  tft.drawString(String(displayedSpeed), 120, 80);
}

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_24);

  tft.drawString("Turn:", 10, 10);
  tft.drawString("Step:", 10, 40);
  tft.drawString("Speed:", 10, 80);

  drawDirectionButtons();
  drawSpeedButtons();
  drawStartStopButton();
}

void logo() {
  tft.fillScreen(TFT_GREEN);
  int centerX = 120;
  int centerY = 160;
  int radius = 70;
  int thickness = 7;

  tft.fillCircle(centerX, centerY, radius, TFT_RED);
  for (int r = radius - thickness / 2; r <= radius + thickness / 2; r++) {
    tft.drawCircle(centerX, centerY, r, TFT_BLACK);
  }
  tft.setTextColor(TFT_BLACK, TFT_RED);
  tft.setFreeFont(&FreeSansBold24pt7b);
  tft.drawString("DSN", 70, 142);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.drawString("Stepper motor Control", 20, 300);
}