# CycloSort

Project Overview: High-Precision 6-Axis Sorting Arm
This repository contains the Java host controller and Arduino firmware for a custom-engineered robotic sorting system. The project utilizes computer vision to identify and sort hardware (nuts/bolts) using a hybrid mechanical transmission system.

Key Technical Features
Mechanical Drive: Features a custom-designed 19:1 cycloidal gearbox for the base to provide high torque and zero-backlash rotation, paired with a belt-driven orbital shoulder.

Hybrid Power: Utilizes a 6-stepper motor configuration (NEMA 17) powered by a 12V 10A supply, ensuring constant holding torque for the 1/8" aluminum arm structure.

Control Logic: The "Brain" is a Java-based application developed in IntelliJ, handling Inverse Kinematics (IK) and trapezoidal motion profiling (acceleration/deceleration ramps).

Hardware Interface: Employs an Arduino Uno R3 as a slave controller, receiving high-level motion commands via Serial Communication to pulse DRV8825 drivers with 1/16 microstepping.

Vision System: Integrated with a stationary overhead camera to map pixel coordinates to real-world motor steps for autonomous object manipulation.

🛠 Hardware Used
Motors: 4x High-Torque NEMA 17, 2x Slim NEMA 17.

Materials: Pocketed 1/8" Aluminum plates, 3D-printed cycloidal components, 4mm Steel dowel pins.

Electronics: Arduino Uno R3, DRV8825 Stepper Drivers, 12V 10A Switching Power Supply.
