#include <AccelStepper.h>

// Define step and direction pins for 6 motors
#define BASE_STEP_PIN 2
#define BASE_DIR_PIN 3

#define SHOULDER_STEP_PIN 4
#define SHOULDER_DIR_PIN 5

#define MOTOR3_STEP_PIN 6
#define MOTOR3_DIR_PIN 7

#define MOTOR4_STEP_PIN 8
#define MOTOR4_DIR_PIN 9

#define MOTOR5_STEP_PIN 10
#define MOTOR5_DIR_PIN 11

#define MOTOR6_STEP_PIN 12
#define MOTOR6_DIR_PIN 13

// Instantiate 6 AccelStepper objects (1 = DRIVER interface)
AccelStepper baseStepper(AccelStepper::DRIVER, BASE_STEP_PIN, BASE_DIR_PIN);
AccelStepper shoulderStepper(AccelStepper::DRIVER, SHOULDER_STEP_PIN, SHOULDER_DIR_PIN);
AccelStepper motor3(AccelStepper::DRIVER, MOTOR3_STEP_PIN, MOTOR3_DIR_PIN);
AccelStepper motor4(AccelStepper::DRIVER, MOTOR4_STEP_PIN, MOTOR4_DIR_PIN);
AccelStepper motor5(AccelStepper::DRIVER, MOTOR5_STEP_PIN, MOTOR5_DIR_PIN);
AccelStepper motor6(AccelStepper::DRIVER, MOTOR6_STEP_PIN, MOTOR6_DIR_PIN);

String inputBuffer = "";

void setup() {
  Serial.begin(115200);

  // Configure max speed and acceleration for all motors
  baseStepper.setMaxSpeed(1000);
  baseStepper.setAcceleration(500);

  shoulderStepper.setMaxSpeed(1000);
  shoulderStepper.setAcceleration(500);

  motor3.setMaxSpeed(1000);
  motor3.setAcceleration(500);

  motor4.setMaxSpeed(1000);
  motor4.setAcceleration(500);

  motor5.setMaxSpeed(1000);
  motor5.setAcceleration(500);

  motor6.setMaxSpeed(1000);
  motor6.setAcceleration(500);
}

void parseCommand(String cmd) {
  cmd.trim();
  int bIndex = cmd.indexOf('B');
  int sIndex = cmd.indexOf('S');

  if (bIndex != -1) {
    int endB = cmd.indexOf(' ', bIndex);
    if (endB == -1) endB = cmd.length();
    long bVal = cmd.substring(bIndex + 1, endB).toInt();
    baseStepper.moveTo(bVal);
  }

  if (sIndex != -1) {
    int endS = cmd.indexOf(' ', sIndex);
    if (endS == -1) endS = cmd.length();
    long sVal = cmd.substring(sIndex + 1, endS).toInt();
    shoulderStepper.moveTo(sVal);
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

  // Continuously run all steppers to ensure smooth acceleration
  baseStepper.run();
  shoulderStepper.run();
  motor3.run();
  motor4.run();
  motor5.run();
  motor6.run();
}
