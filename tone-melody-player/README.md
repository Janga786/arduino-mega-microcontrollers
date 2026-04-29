# Tone Melody Player

A single-button melody player: hold the button to play the song,
release to stop.  Uses a fixed half-period table to set the speaker
output frequency by hand, so each note's pitch is exactly what you
expect rather than rounded by the Arduino `tone()` quantiser.

The song is *We're Off to See the Wizard*, encoded as a 25-element
note + rhythm pair so the timing is data-driven.  Stripping out the
melody table and dropping in a different one switches songs in two
lines of code.

<p align="center">
<img src="docs/morse_demo.mp4" alt="Demo" width="60%"/>
</p>

A short clip is in [`docs/morse_demo.mp4`](docs/morse_demo.mp4) —
filmed on the same hardware running an SOS Morse-code variant of
this sketch.

## What it does

- Pin 9 is the button input (pulled up; press to assert).
- Pin 5 is the speaker output (toggled at half the note frequency).
- A `melody[]` array holds each note's half-period in microseconds
  (`NOTE_C5 = 4780`, etc.), and a `rhythm[]` array holds each note's
  duration in arbitrary tempo units (`EIGHTH = 150`,
  `QUARTER = 300`, etc.).
- The main loop walks both arrays in lock-step, using
  `tone()`-compatible timing but driving the pin directly.

## What's in this folder

```
tone-melody-player/
├── src/
│   └── melody_player/
│       └── melody_player.ino       # 84 lines
└── docs/
    └── morse_demo.mp4              # 12 s clip on the same hardware
```

## How to extend it

This is the simplest entry-point in the repo — the natural extensions
are the music-box / music-game sketches in the
[`interrupt-music-box/`](../interrupt-music-box/) project, which
replace the busy-wait song loop with a Timer3 output-compare ISR
running concurrently with the input scan.
