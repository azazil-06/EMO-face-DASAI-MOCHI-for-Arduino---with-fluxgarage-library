# Robot Eyes Project

<p align="center">
  <img src="Screenshot_20260510-203344.png" width="300" />
  <img src="Screenshot_20260510-203355.png" width="300" />
</p>

A fun Arduino project featuring a robot with emotional expressions and interactive touch logic. This project uses an OLED display to bring a robot face to life with various moods and animations.

## Features
- **Emotions & Moods**: The robot can express various feelings like HAPPY, ANGRY, TIRED, and even show heart-eyes!
- **Touch Interaction**: Responds to single taps, multi-taps, and long presses.
- **Power Management**: Includes a custom shutdown and turn-on sequence triggered by a 10-second long press.
- **Idle Animations**: The robot looks around, blinks, and gets curious when left alone.
- **Sound Effects**: Integrated buzzer for R2D2-style chirps, screams, and sad tones.

## Hardware Used
- Arduino Nano
- SSD1306 OLED Display (128x64)
- Capacitive Touch Sensor
- Passive Buzzer

## Library Credit
This project relies heavily on the excellent **RoboEyes** library by **Flux Garage**. You can find it here:
[https://github.com/FluxGarage/RoboEyes.git](https://github.com/FluxGarage/RoboEyes.git)

Big thanks to Flux Garage for making robot eyes so easy to implement!

## How to Use
1. Install the `Adafruit_SSD1306` and `FluxGarage_RoboEyes` libraries in your Arduino IDE.
2. Connect your hardware according to the pin definitions:
    - **SCL**: A5
    - **SDA**: A4
    - **Buzzer**: Pin D6 & D7 (low)
    - **Sensor**: A5
3. Upload the code and start interacting with your robot!
