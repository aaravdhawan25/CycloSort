#include <AccelStepper.h>

// Define step and direction pins for 6 motors
#define BASE_STEP_PIN 2
#define BASE_DIR_PIN 3

#define SHOULDER_STEP_PIN 4
#define SHOULDER_DIR_PIN 5

#define L_STEP_PIN 6
#define L_DIR_PIN 7

#define E_STEP_PIN 8
#define E_DIR_PIN 9

#define U_STEP_PIN 10
#define U_DIR_PIN 11

#define R_STEP_PIN 12
#define R_DIR_PIN 13

// Optional: Enable pins if used, but for now we can enable/disable via AccelStepper if supported,
// or just handle a logical state. For simplicity, we'll assume we can use standard enable logic if wired,
// or we just enable/disable the outputs using standard Arduino digitalWrite.
// However, since we used pins 2-13, we don't have enough pins defined for Enable for all 6 motors.
// We will mock the Enable/Disable state or implement a simple boolean if no pins are wired.
bool motorsEnabled = true;

// Instantiate 6 AccelStepper objects (1 = DRIVER interface)
AccelStepper baseStepper(AccelStepper::DRIVER, BASE_STEP_PIN, BASE_DIR_PIN);
AccelStepper shoulderStepper(AccelStepper::DRIVER, SHOULDER_STEP_PIN, SHOULDER_DIR_PIN);
AccelStepper lStepper(AccelStepper::DRIVER, L_STEP_PIN, L_DIR_PIN);
AccelStepper eStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
AccelStepper uStepper(AccelStepper::DRIVER, U_STEP_PIN, U_DIR_PIN);
AccelStepper rStepper(AccelStepper::DRIVER, R_STEP_PIN, R_DIR_PIN);

String inputBuffer = "";
bool isMoving = false;

void setup() {
  Serial.begin(115200);

  // Configure max speed and acceleration for all motors
  baseStepper.setMaxSpeed(1000);
  baseStepper.setAcceleration(500);

  shoulderStepper.setMaxSpeed(1000);
  shoulderStepper.setAcceleration(500);

  lStepper.setMaxSpeed(1000);
  lStepper.setAcceleration(500);

  eStepper.setMaxSpeed(1000);
  eStepper.setAcceleration(500);

  uStepper.setMaxSpeed(1000);
  uStepper.setAcceleration(500);

  rStepper.setMaxSpeed(1000);
  rStepper.setAcceleration(500);
}

void parseCommand(String cmd) {
  cmd.trim();

  if (cmd == "E1") {
    motorsEnabled = true;
    baseStepper.enableOutputs();
    shoulderStepper.enableOutputs();
    lStepper.enableOutputs();
    eStepper.enableOutputs();
    uStepper.enableOutputs();
    rStepper.enableOutputs();
    return;
  }

  if (cmd == "E0") {
    motorsEnabled = false;
    baseStepper.disableOutputs();
    shoulderStepper.disableOutputs();
    lStepper.disableOutputs();
    eStepper.disableOutputs();
    uStepper.disableOutputs();
    rStepper.disableOutputs();
    return;
  }

  // Parse movement commands like B100 S50 L10 E-20 U0 R5
  int bIndex = cmd.indexOf('B');
  int sIndex = cmd.indexOf('S');
  int lIndex = cmd.indexOf('L');
  int eIndex = cmd.indexOf('E');
  int uIndex = cmd.indexOf('U');
  int rIndex = cmd.indexOf('R');

  if (bIndex != -1) {
    int endIdx = cmd.indexOf(' ', bIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(bIndex + 1, endIdx).toInt();
    baseStepper.moveTo(val);
    isMoving = true;
  }

  if (sIndex != -1) {
    int endIdx = cmd.indexOf(' ', sIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(sIndex + 1, endIdx).toInt();
    shoulderStepper.moveTo(val);
    isMoving = true;
  }

  if (lIndex != -1) {
    int endIdx = cmd.indexOf(' ', lIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(lIndex + 1, endIdx).toInt();
    lStepper.moveTo(val);
    isMoving = true;
  }

  if (eIndex != -1) {
    int endIdx = cmd.indexOf(' ', eIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(eIndex + 1, endIdx).toInt();
    eStepper.moveTo(val);
    isMoving = true;
  }

  if (uIndex != -1) {
    int endIdx = cmd.indexOf(' ', uIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(uIndex + 1, endIdx).toInt();
    uStepper.moveTo(val);
    isMoving = true;
  }

  if (rIndex != -1) {
    int endIdx = cmd.indexOf(' ', rIndex);
    if (endIdx == -1) endIdx = cmd.length();
    long val = cmd.substring(rIndex + 1, endIdx).toInt();
    rStepper.moveTo(val);
    isMoving = true;
  }
}

void loop() {
  // Non-blocking serial read to ensure smooth stepping
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        parseCommand(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }

  if (motorsEnabled) {
    baseStepper.run();
    shoulderStepper.run();
    lStepper.run();
    eStepper.run();
    uStepper.run();
    rStepper.run();
  }

  // Check if all motors have finished moving
  if (isMoving &&
      baseStepper.distanceToGo() == 0 &&
      shoulderStepper.distanceToGo() == 0 &&
      lStepper.distanceToGo() == 0 &&
      eStepper.distanceToGo() == 0 &&
      uStepper.distanceToGo() == 0 &&
      rStepper.distanceToGo() == 0) {

    isMoving = false;
    Serial.print("READY\n");
  }
}
