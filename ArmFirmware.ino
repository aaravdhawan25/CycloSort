#include <AccelStepper.h>

// --- THE "HIDDEN" DIGITAL PINS ---
#define ENABLE_PIN A0   // Using Analog A0 as a Digital Output
#define LIMIT_SWITCH A1 // Using Analog A1 as a Digital Input

// --- MOTOR PINS (2-13) ---
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

// Instantiate motors
AccelStepper baseM(1, BASE_STEP, BASE_DIR);
AccelStepper shoulderM(1, SHOULDER_STEP, SHOULDER_DIR);
AccelStepper sliderM(1, SLIDER_STEP, SLIDER_DIR);
AccelStepper elbowM(1, ELBOW_STEP, ELBOW_DIR);
AccelStepper wristUDM(1, WRIST_UD_STEP, WRIST_UD_DIR);

void setup() {
  Serial.begin(115200);

  // Set up the hidden digital pins
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // Enable drivers

  pinMode(LIMIT_SWITCH,
          INPUT_PULLUP); // "Pullup" means it's safe and doesn't need a resistor

  // Initial settings (same as before)
  baseM.setMaxSpeed(2000);
  baseM.setAcceleration(800);
  // ... (all other motors) ...
}

void loop() {
  // Check limit switch: If pressed, stop the slider immediately!
  // Optimized to minimize CPU cycles:
  // 1. Only read the limit switch if the slider is currently moving.
  // 2. Avoid redundant stop() calls by tracking if the switch has already
  //    triggered a stop for the current movement.
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
      lastTarget =
          sliderM.targetPosition(); // Update to the new deceleration target
    }
  }

  // ... (Rest of your Jules serial logic and run() commands) ...
}
