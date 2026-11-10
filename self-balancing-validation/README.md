# Self-Balancing Robot Bring-Up Sketches

Two short sketches for checking a self-balancing robot's parts one at a
time before closing the control loop.

| Sketch | What it does |
| --- | --- |
| `mpu6050-gyro-test/gyro_test.ino` (82 lines) | Starts an MPU-6050 with the Adafruit library (±500 °/s gyro range, 21 Hz low-pass). Averages 500 gyro readings taken 50 ms apart (about 25 s) as the bias, then prints bias-corrected X/Y/Z rates in °/s, tab-separated for the Serial Plotter. |
| `ultrasonic-motor-driver/motor_driver_test.ino` (17 lines) | Drives one TB6612FNG channel forward at 50 % PWM: PWM on pin 3, AIN1/AIN2 on pins 35/34, standby on pin 36. Despite the folder name, it does not use an ultrasonic sensor. |

The gyro sketch's on-screen prompt says to hold still for 3 seconds, but
the loop takes about 25 seconds.

## Hardware

ATmega2560 board, MPU-6050 on I²C (SDA 20, SCL 21), TB6612FNG driver and
a DC motor.
