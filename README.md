<div align="center">

# Arduino Mega Microcontroller Portfolio

**Six self-contained microcontroller projects on the ATmega2560: a custom Arduino-Mega-class PCB designed in Eagle and exported as production-ready Gerbers, a bare-metal 40 kHz ADC sampler, a hand-rolled IR link-layer protocol, an interrupt-driven music game, a tone-table melody player, and a sensor-validation suite for a self-balancing robot.**

[![C++](https://img.shields.io/badge/C%2B%2B-Arduino-00979d.svg)](#)
[![MCU: ATmega2560](https://img.shields.io/badge/MCU-ATmega2560-3a86ff.svg)](https://www.microchip.com/en-us/product/ATmega2560)
[![Toolchain](https://img.shields.io/badge/toolchain-Arduino%202.x-9b59b6.svg)](https://www.arduino.cc/en/software)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

</div>

---

## Contents

- [Overview](#overview)
- [The projects](#the-projects)
- [Repository layout](#repository-layout)
- [Building (any project)](#building-any-project)
- [License](#license)

---

## Overview

This repository is a curated showcase of six embedded-systems projects
targeting the **Arduino Mega 2560** (Microchip ATmega2560 @ 16 MHz),
covering the design surface from custom-PCB hardware design through
bare-metal register-level firmware up to multi-sensor robotics
integration.

The projects span four categories:

| Category | Projects |
| --- | --- |
| **Hardware design**  | `custom-mega-pcb/` |
| **Bare-metal firmware** | `bare-metal-adc-sampler/`, `interrupt-music-box/` |
| **Protocol design**  | `ir-link-layer-protocol/` |
| **Robotics integration** | `self-balancing-validation/`, `tone-melody-player/` |

> **What you can read off this repository.** Every project here is
> small enough to read in ten minutes and demonstrates a specific
> embedded-systems skill: **PCB schematic capture and Gerber export**
> (custom-mega-pcb), **direct AVR register access without the Arduino
> abstraction layer** (bare-metal-adc-sampler), **multi-vector
> interrupt coordination** (interrupt-music-box), **link-layer
> protocol design** (ir-link-layer-protocol), and **smoke-test-driven
> sensor bring-up** (self-balancing-validation).

---

## The projects

### 1. `custom-mega-pcb/` — Skyboard, an Arduino-Mega-class development board

A 4-layer Eagle CAD design of an Arduino-Mega-2560-class board:
ATmega2560 @ 16 MHz, USB-Serial via ATmega16U2, full pin breakout,
LDO power section, and Mega-shield-compatible header layout.  Ships
schematic, board file, full Gerber output, drill files, and pick-and-
place files for both sides — i.e. everything a fab needs to
manufacture the board.

[Project README →](custom-mega-pcb/README.md)

### 2. `bare-metal-adc-sampler/` — 40 kHz ADC sampling without `analogRead()`

A direct-register-access ATmega2560 ADC sampler with deterministic
25 µs sample period, a Timer1 CTC interrupt as the only conversion
trigger, a 256-entry ring buffer, and inline threshold-based event
detection.  Demonstrates ISR-safe lock-free producer/consumer
patterns and bare-metal AVR programming.

[Project README →](bare-metal-adc-sampler/README.md)

### 3. `ir-link-layer-protocol/` — A hand-built IR data link

A bidirectional link-layer protocol over a 38 kHz IR carrier between
two ATmega2560 boards: source/destination addressing, broadcast,
runtime baud-rate selection, framed packets with XOR checksum
validation, and an input-capture-driven receive state machine.  Not
NEC / RC-5 — a custom protocol designed for **point-to-point data
transfer** rather than remote-control button presses.

[Project README →](ir-link-layer-protocol/README.md)

### 4. `interrupt-music-box/` — Three timer-driven sketches, escalating

Starts with a 3-note speaker driven by a Timer3 output-compare ISR
(no `tone()`, no `delay()`), escalates to a Simon-says music game
with debounced button input and a scoring state machine, finishes
with a refined version that adds a runtime-configurable input window.
Demonstrates concurrent ISR coordination across three vector
groups.

[Project README →](interrupt-music-box/README.md)

### 5. `tone-melody-player/` — Single-button melody player

A simple gateway project: hold a button → play *We're Off to See the
Wizard*.  Uses a hand-rolled half-period table for exact pitches
rather than the Arduino `tone()` quantiser.  Ships a 12-second demo
clip.

[Project README →](tone-melody-player/README.md)

### 6. `self-balancing-validation/` — Pre-integration smoke tests

Three short Arduino sketches that bring up the individual peripherals
of a self-balancing robot — MPU-6050 IMU, TB6612FNG H-bridge driver,
and the regulator's power rails — *before* the closed-loop controller
is ever turned on.  Validation sketches prevent the
"three-sensors-and-two-motors-all-fail-at-once" debugging trap.

[Project README →](self-balancing-validation/README.md)

---

## Repository layout

```
arduino-mega-microcontrollers/
├── README.md                           # this file
├── LICENSE
│
├── custom-mega-pcb/                    # 1. Skyboard — custom PCB design
│   ├── schematic/                      #    .sch + .brd
│   ├── gerbers/                        #    production-ready outputs
│   └── docs/                           #    layout PNGs + ERC report
│
├── bare-metal-adc-sampler/             # 2. 40 kHz ADC sampler
│   └── src/adc_sampler/
│
├── ir-link-layer-protocol/             # 3. IR link-layer protocol
│   └── src/ir_link/
│
├── interrupt-music-box/                # 4. Timer-driven music sketches
│   ├── src/music_box/
│   ├── src/music_game/
│   ├── src/music_game_advanced/
│   └── docs/                           #    speaker-output demo
│
├── tone-melody-player/                 # 5. Button-triggered melody
│   ├── src/melody_player/
│   └── docs/                           #    12-second demo clip
│
└── self-balancing-validation/          # 6. Robot peripheral smoke tests
    ├── mpu6050-gyro-test/
    ├── ultrasonic-motor-driver/
    └── voltage-regulator/
```

---

## Building (any project)

1. Install **Arduino IDE 2.x** (or `arduino-cli` if you prefer the
   command line).
2. In *Boards Manager*, install the *Arduino Mega 2560* board
   package.
3. Open the project's `src/<project>/<project>.ino` in the IDE.
4. Select **Tools → Board → Arduino Mega or Mega 2560**.
5. Plug in the Mega over USB, select the right serial port under
   **Tools → Port**, and click **Upload**.

The PCB project (`custom-mega-pcb/`) is read-only as far as Arduino
is concerned — it ships the Eagle source files and the Gerber output.
To open the design, use **Eagle CAD 9.x** (legacy free version) or
import into KiCad via the built-in Eagle importer.

---

## License

Software in this repository is released under the [MIT License](LICENSE).
