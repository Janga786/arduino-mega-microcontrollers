# Button Melody Player

A short ATmega2560 sketch: press the button on pin 9 and the board plays a
25-note melody (the notes of *Happy Birthday*) on a speaker on pin 5.
Each note is a square wave made by toggling the pin with a
`nop` delay loop; `melody[]` and `rhythm[]` arrays hold the notes and
their lengths, so another tune is a data change.

This is the simple busy-wait version. The
[`interrupt-music-box/`](../interrupt-music-box/) sketches do the same job
with timer interrupts.

A 12-second clip of the course board and speaker is in
[`docs/morse_demo.mp4`](docs/morse_demo.mp4) (no audio track).

## Files

`src/melody_player/melody_player.ino` (84 lines).
