#include <AccelStepper.h>

// --- DIGITAL PINS ---
#define ENABLE_PIN 12   // Using 12 as global enable
#define LIMIT_SWITCH A0 // Using Analog A0 as a Digital Input

// --- MOTOR PINS ---
// Avoid pin 13 to prevent startup flicker
#define BASE_STEP 2
#define BASE_DIR 3
#define SHOULDER_STEP 4
#define SHOULDER_DIR 5
#define SLIDER_STEP 6
#define SLIDER_DIR 7
#define ELBOW_STEP 8
#define ELBOW_DIR 9
#define WRIST_UD_STEP 10
#define WRIST_UD_DIR 11
#define WRIST_ROT_STEP A1
#define WRIST_ROT_DIR A2

// Instantiate motors
AccelStepper baseM(1, BASE_STEP, BASE_DIR);
AccelStepper shoulderM(1, SHOULDER_STEP, SHOULDER_DIR);
AccelStepper sliderM(1, SLIDER_STEP, SLIDER_DIR);
AccelStepper elbowM(1, ELBOW_STEP, ELBOW_DIR);
AccelStepper wristUDM(1, WRIST_UD_STEP, WRIST_UD_DIR);
AccelStepper wristRotM(1, WRIST_ROT_STEP, WRIST_ROT_DIR);

String inputString = "";
bool stringComplete = false;
bool isMoving = false;

void setup() {
  Serial.begin(115200);

  // Set up the hidden digital pins
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // Disable drivers initially (assuming LOW is enable)

  pinMode(LIMIT_SWITCH, INPUT_PULLUP);

  // Initial settings (same as before)
  baseM.setMaxSpeed(2000);
  baseM.setAcceleration(800);

  shoulderM.setMaxSpeed(2000);
  shoulderM.setAcceleration(800);

  sliderM.setMaxSpeed(2000);
  sliderM.setAcceleration(800);

  elbowM.setMaxSpeed(2000);
  elbowM.setAcceleration(800);

  wristUDM.setMaxSpeed(2000);
  wristUDM.setAcceleration(800);

  wristRotM.setMaxSpeed(2000);
  wristRotM.setAcceleration(800);
}

void loop() {
  // Read Serial Commands
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  if (stringComplete) {
    inputString.trim();

    if (inputString == "E1") {
      digitalWrite(ENABLE_PIN, LOW); // Enable drivers
    } else if (inputString == "E0") {
      digitalWrite(ENABLE_PIN, HIGH); // Disable drivers
    } else if (inputString.length() > 0) {
      // Parse coordinates: B S L E U R
      long b = baseM.targetPosition();
      long s = shoulderM.targetPosition();
      long l = sliderM.targetPosition();
      long e = elbowM.targetPosition();
      long u = wristUDM.targetPosition();
      long r = wristRotM.targetPosition();

      int bIdx = inputString.indexOf('B');
      int sIdx = inputString.indexOf('S');
      int lIdx = inputString.indexOf('L');
      int eIdx = inputString.indexOf('E');
      int uIdx = inputString.indexOf('U');
      int rIdx = inputString.indexOf('R');

      if (bIdx >= 0) { b = inputString.substring(bIdx + 1).toInt(); }
      if (sIdx >= 0) { s = inputString.substring(sIdx + 1).toInt(); }
      if (lIdx >= 0) { l = inputString.substring(lIdx + 1).toInt(); }
      if (eIdx >= 0) { e = inputString.substring(eIdx + 1).toInt(); }
      if (uIdx >= 0) { u = inputString.substring(uIdx + 1).toInt(); }
      if (rIdx >= 0) { r = inputString.substring(rIdx + 1).toInt(); }

      baseM.moveTo(b);
      shoulderM.moveTo(s);
      sliderM.moveTo(l);
      elbowM.moveTo(e);
      wristUDM.moveTo(u);
      wristRotM.moveTo(r);

      isMoving = true;
    }

    inputString = "";
    stringComplete = false;
  }

  // Check limit switch: If pressed, stop the slider immediately!
  static long lastTarget = 0;
  static bool limitTriggered = false;

  long currentTarget = sliderM.targetPosition();

  // Reset the trigger if the target position changes (new movement)
  if (currentTarget != lastTarget) {
    limitTriggered = false;
    lastTarget = currentTarget;
  }

  if (sliderM.distanceToGo() != 0 && !limitTriggered) {
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      sliderM.stop();
      limitTriggered = true;
      lastTarget = sliderM.targetPosition(); // Update to the new deceleration target
    }
  }

  // Run all motors
  baseM.run();
  shoulderM.run();
  sliderM.run();
  elbowM.run();
  wristUDM.run();
  wristRotM.run();

  // Check if movement is done
  if (isMoving &&
      baseM.distanceToGo() == 0 &&
      shoulderM.distanceToGo() == 0 &&
      sliderM.distanceToGo() == 0 &&
      elbowM.distanceToGo() == 0 &&
      wristUDM.distanceToGo() == 0 &&
      wristRotM.distanceToGo() == 0) {

    Serial.println("READY");
    isMoving = false;
  }
}
