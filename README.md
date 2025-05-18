# Touchscreen Stepper Motor Control with ESP32-S3
this prepaing for 28-BYJ Stepper motor control by ILI9341 with Esp32

This project demonstrates how to control a 28BYJ-48 stepper motor using an ILI9341 touch display and an ESP32-S3 microcontroller. It provides an interactive interface to control the motor's direction and speed.

## Project Overview

This project utilizes a low-cost 28BYJ-48 stepper motor, the powerful ESP32-S3 development board, and an ILI9341 touch display to create a user-friendly stepper motor control system. 
The touchscreen interface allows for easy adjustment of the motor's speed (within a 0-100 range) and direction, as well as a simple start/stop functionality.

video : https://youtu.be/Szm2ziKMi20

## Hardware Used

* **Microcontroller:** ESP32-S3
* **Display:** ILI9341 Touch Display
* **Stepper Motor:** 28BYJ-48
* **Motor Driver:** ULN2003 (or equivalent)
* **Connecting Wires**

## Software Used

* **Arduino IDE**
* **Libraries:**
    * [TFT_eSPI]([Link to TFT_eSPI Library]) - For controlling the ILI9341 display.
    * [AccelStepper]([Link to AccelStepper Library]) - For controlling the stepper motor with acceleration and speed management.

## Wiring Diagram (Conceptual)

ESP32-S3 Pin  &lt;-->  ILI9341 Pin (SPI, Touch Pins)
ESP32-S3 Pin  &lt;-->  ULN2003 Input 1 (Motor Coil 1)
ESP32-S3 Pin  &lt;-->  ULN2003 Input 2 (Motor Coil 3)
ESP32-S3 Pin  &lt;-->  ULN2003 Input 3 (Motor Coil 2)
ESP32-S3 Pin  &lt;-->  ULN2003 Input 4 (Motor Coil 4)
ULN2003 COM   &lt;-->  +5V Power Supply (for motor)
GND           &lt;-->  GND (for all components)

**Note:** Refer to the specific datasheets for the ILI9341, ESP32-S3, and ULN2003 for accurate pin assignments. The motor coil connections to the ULN2003 inputs might need adjustment based on your specific wiring.

## Installation and Usage

1.  **Prerequisites:**
    * Install the Arduino IDE.
    * Install the ESP32 board support in the Arduino IDE (if you haven't already).
    * Install the `TFT_eSPI` and `AccelStepper` libraries using the Arduino Library Manager (`Sketch` -> `Include Library` -> `Manage Libraries...`).

2.  **Wiring:** Connect the ESP32-S3, ILI9341 display, and ULN2003 motor driver to the 28BYJ-48 stepper motor according to the wiring diagram (and your specific hardware setup).

3.  **Code:**
    * Copy the provided Arduino code (`.ino` file) to your Arduino IDE.
    * **Important:** Review and adjust the pin definitions at the beginning of the code (`motorPin1`, `motorPin2`, `motorPin3`, `motorPin4`) to match your wiring to the ULN2003 driver.
    * **Optional:** Modify the `maxMotorSpeed` and `minMotorSpeed` constants to suit the capabilities of your motor and desired speed range.

4.  **Upload:** Connect your ESP32-S3 to your computer via USB and upload the code to the board using the Arduino IDE (`Sketch` -> `Upload`).

5.  **Operation:**
    * Upon powering on, the display might prompt for touch calibration. Follow the on-screen instructions.
    * The main screen will display:
        * **Turn:** The number of full rotations.
        * **Step:** The current step within a full rotation (0-2047 for 2048 steps per turn).
        * **Speed:** An adjustable speed value ranging from 0 to 100.
    * **Speed Control:** Use the "+" and "-" buttons on the screen to increase or decrease the target speed.
    * **Direction Control:** Tap the "CW" (Clockwise) or "CCW" (Counter-Clockwise) buttons to set the motor's direction.
    * **Start/Stop:** Tap the "START" button to begin motor rotation at the set speed and direction. Tap "STOP" to halt the motor.

## Code Structure

The Arduino code utilizes the `TFT_eSPI` library for display control and the `AccelStepper` library for managing the stepper motor's movement, including speed and acceleration.

* **Pin Definitions:** Defines the GPIO pins connected to the motor driver and display.
* **`AccelStepper` Initialization:** Creates an `AccelStepper` object to control the motor.
* **TFT Initialization:** Initializes the `TFT_eSPI` library and sets up the display.
* **UI Drawing Functions:** Functions to draw the buttons, text, and interactive elements on the touchscreen.
* **Touch Handling:** Detects touch events on the screen and responds to button presses.
* **Motor Control:** Uses `AccelStepper` functions like `setMaxSpeed()`, `setSpeed()`, `moveTo()`, and `run()` to control the motor's movement based on user input.
* **Counter Update:** Tracks the number of turns and steps and updates the display accordingly (only when the values change).

## Notes

* The stepper motor is driven in **full-step mode** in the current code. You can explore other driving modes (like half-step) by modifying the `AccelStepper` initialization and potentially the wiring.
* The speed control (0-100) is mapped to a range of actual motor speeds defined by `minMotorSpeed` and `maxMotorSpeed`. Adjust these values based on your motor's capabilities.
* This code is designed to run on the ESP32 and utilizes its multitasking capabilities through the `AccelStepper` library.
