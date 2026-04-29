# Self-Balancing Robot — Sensor & Actuator Validation Suite

Three small Arduino sketches I wrote to **smoke-test the individual
peripherals** of a self-balancing robot before integrating them into
a closed-loop controller.  Each sketch exercises one subsystem in
isolation, prints diagnostic output, and is intentionally short
(<100 lines) so a failure is unambiguous.

The point of validation sketches is that, *before* you debug a feedback
loop with three sensors and two motors all interacting, you've already
proved each component works on its own.  When the integrated system
misbehaves, the search space is small.

## The three sketches

### `mpu6050-gyro-test/` — IMU smoke test

Brings up an MPU-6050 6-axis IMU over I²C, reads accelerometer + gyro
+ temperature at the default 1 kHz internal rate, and prints the
output to the serial console at 9600 baud.

Sets:
- Gyroscope range to ±500 °/s (good for hand-held testing).
- Digital low-pass filter to 21 Hz (rejects motor brushwork noise).
- Auto-calibrates the gyro X/Y/Z bias by averaging 50 samples on
  start-up (assumes the robot is stationary at boot).

This is the sketch I run **first** on any new build, before the
motors are even bolted on, because if the IMU isn't reporting then
nothing downstream can possibly work.

### `ultrasonic-motor-driver/` — TB6612FNG smoke test

The shortest sketch in the repo (17 lines): drives one channel of a
TB6612FNG dual H-bridge motor driver at 50 % duty cycle indefinitely,
with the standby pin held high.  Lets you confirm:

- The motor wiring polarity (does it spin forward or backward?).
- The PWM pin is on the same `Timer0` / `Timer1` group as the rest of
  the system uses (so you don't accidentally take down `millis()`
  by re-prescaling the timer).
- The standby pin is correctly pulled high (a common bring-up
  mistake — if standby is left floating, the H-bridge looks dead).

### `voltage-regulator/` — power-rail smoke test

A scope-only sketch that exercises the regulator under a varying
load by toggling a known dummy resistive load on a GPIO.  Used in
combination with a multimeter / scope on the 5 V and 3.3 V rails to
validate that the LDO can sustain the worst-case current draw
(motors stalled + IMU active + ultrasonic sensor pinging) without
sagging below the MCU's brown-out threshold.

## Why publish "validation sketches"?

Most public Arduino projects ship the integrated firmware only,
which makes them brittle to copy: anyone trying to reproduce the
build has to debug ten things at once when it doesn't work
first-try.  Publishing the validation sketches **alongside** the
integrated firmware is good practice — if the gyro test passes but
the integrated controller doesn't, the bug is in the controller,
not the IMU.

## What's in this folder

```
self-balancing-validation/
├── mpu6050-gyro-test/
│   └── gyro_test.ino                  # 82 lines — IMU bring-up
├── ultrasonic-motor-driver/
│   └── motor_driver_test.ino          # 17 lines — H-bridge bring-up
└── voltage-regulator/                 # placeholder for the rail-test sketch
```

## Hardware

- Arduino Mega 2560 or compatible.
- MPU-6050 IMU on the I²C bus (SDA = 20, SCL = 21 on the Mega).
- TB6612FNG dual H-bridge driver, with PWM on pin 3 and direction
  control on pins 34 / 35, standby on pin 36.
- A potentiometer-loaded power rail for the voltage-regulator test.
