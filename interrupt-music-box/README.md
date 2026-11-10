# Timer-Interrupt Music Box

Three CE 351 sketches that play notes on a speaker from timer interrupts on
the ATmega2560. Commands come in over the serial monitor (9600 baud).
`music_game.ino` and `music_game_advanced.ino` started from a lab template;
their `// Your Code //` and "Inside here, you need to..." comments come from
it.

A 12-second clip of the course board running one of these sketches is in
[`docs/speaker_demo.mp4`](docs/speaker_demo.mp4). The clip has no audio track.

## The sketches

| Sketch | What it does |
| --- | --- |
| `src/music_box/music_box.ino` (108 lines) | Timer1 overflow interrupt toggles the speaker pin, reloading `TCNT1` with a half-period from a 3-entry table. Serial `n`/`N` start and stop the note, `i` steps to the next note. |
| `src/music_game_advanced/music_game_advanced.ino` (184 lines) | Song player. Timer1 (no prescaler) sets the pitch; Timer3 (prescaler 256, 62 500 counts = 1 s) runs a note/rest state machine through note and duration tables. Serial `p` plays a 17-note song. |
| `src/music_game/music_game.ino` (240 lines) | The song player plus a rhythm-copy game. Serial `r` plays a 3-note pattern, then the player repeats it on the button; each press interval must be within ±10 000 Timer3 counts (160 ms) of the pattern. It then plays a win or lose tune. |

Despite its name, `music_game_advanced` is the simpler song-player version.

## Known issues

- Line 1 of `music_game.ino` is the bare text `The music game`, which is
  not a comment, so that sketch does not compile until the line is removed
  or commented out.
- `music_game_advanced.ino` does not compile either: the `else {` before
  the rest-reload lines (around line 165) is missing, so the `}` on line 168
  closes the `switch` early and the `break;` after it falls outside.

## Hardware

An ATmega2560 board, a speaker on pin 5 (PE3), and a push-button on pin 9
(input pull-up). Pin 13 pulses in the `music_box` Timer1 interrupt for scope
checks.
