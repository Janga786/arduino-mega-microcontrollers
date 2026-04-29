# Timer-Interrupt Music Box

Three escalating Arduino Mega sketches that drive a piezo speaker via
**hardware timer interrupts only** — no `tone()`, no `delay()`, no
`millis()`-loops.  The square wave that makes the note is a Timer3
output-compare interrupt; the song's tempo is a separate Timer1
output-compare interrupt; user input is read in a third interrupt
context.

The point is to demonstrate the discipline of separating concerns
across multiple interrupt vectors when you can't afford a busy-wait —
something that gets glossed over by the Arduino reference example.

<p align="center">
<img src="docs/speaker_demo.mp4" alt="Speaker demo" width="60%"/>
</p>

A 12-second clip of the speaker output is in
[`docs/speaker_demo.mp4`](docs/speaker_demo.mp4).

## The three sketches

### 1. `music_box/` — the basics (~108 lines)

A 3-note song (A4, G#4, G4) where each note's half-period is held in
a `uint16_t` table indexed by `noteIndex`.  Timer3's output-compare
ISR toggles the speaker pin at the right rate, Timer1's ISR steps the
note index, and a button on `PCINT1` advances or holds the song.
Three concurrent volatile flags (`playNote`, `incNote`,
`doSomethingBad`) gate the main-loop print queue.

This sketch is the "minimum viable timer-driven sound" — it's the one
to read first.

### 2. `music_game/` — the playable game (~240 lines)

A full **Simon-says music game**: the board plays a 16-note random
melody, the user must echo it back by tapping the button in the
correct rhythm.  Note durations are now timer-counts (`WHO`, `HAL`,
`QUA`, `EIG`, `REST`) so a half-note is exactly 31 250 Timer3 ticks
regardless of clock prescaler.

Adds:
- A 16-note random-sequence generator.
- A debounced button input (rising-edge detector + 50 ms re-arm).
- A scoring state machine (`PLAYBACK → INPUT → JUDGE → REPEAT`).
- Serial-console feedback at 9600 baud.

### 3. `music_game_advanced/` — sharper input window (~184 lines)

A tightened version of the music game with a runtime-configurable
input window (the `DELTA` constant, currently 10 000 Timer3 ticks ≈
160 ms) and per-note miss / hit tracking.  Holding the button pauses
the playback; double-tapping restarts the round.

This is the sketch I'd actually demo from.

## What you can read off these sketches

| Skill | Where it shows up |
| --- | --- |
| Timer1 / Timer3 OC mode setup | top of every `setup()` — `TCCR3A`, `TCCR3B`, `OCR3A` writes |
| ISR-safe shared state | the `volatile uint8_t / bool` declarations near the top |
| Lock-free producer/consumer | the `playNote` / `incNote` / `printFlag` flag handoff |
| Pin-change interrupt for buttons | `PCICR` / `PCMSK1` configuration |
| Running on a 16 MHz crystal | every constant ending in `_TICKS` derives from 16 MHz |
| Avoiding `delay()` | none of the three sketches contain a single `delay()` call |

## What's in this folder

```
interrupt-music-box/
├── src/
│   ├── music_box/
│   │   └── music_box.ino                  # 108 lines, three-note demo
│   ├── music_game/
│   │   └── music_game.ino                 # 240 lines, Simon-says
│   └── music_game_advanced/
│       └── music_game_advanced.ino        # 184 lines, refined version
└── docs/
    └── speaker_demo.mp4                   # ~12 s playback clip
```

## Hardware

- An Arduino Mega 2560 (or any ATmega2560 compatible).
- One piezo or small 8 Ω speaker on pin 5 (`SPEAKER_PIN`).
- One momentary push-button on pin 9 (`BUTTON_PIN`), pulled up.
- Pin 13 (`TEST_PIN`) is toggled at the note-step rate so you can
  scope-verify the timing.
