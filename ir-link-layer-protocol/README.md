# IR Serial Link with Addressing and Checksum

An ATmega2560 sketch that sends typed messages between boards over infrared.
It is UART-style serial over a 38 kHz IR carrier, with a small message
format on top. The same sketch has both the transmit and receive paths.
It is a CE 351 lab: the sketch started from a lab template, and the
`// Your Code//` comments mark the parts students completed.

## How it works

- **Carrier.** Timer5 runs in fast-PWM mode with `ICR5 = 420`
  (16 MHz / 421 = 38.0 kHz) and drives the IR LED on pin 45 (OC5B). The
  transmitter switches the carrier on and off by connecting or disconnecting
  OC5B.
- **Bit timing.** Timer3 overflows once per bit. `bitPeriod[]` holds reload
  values for 300 to 19200 baud; the sketch uses index 2 (2400 baud). The
  rate is set at compile time.
- **Framing.** Each byte goes out as a start bit (carrier on), 8 data bits
  LSB first (a 1 is carrier off, a 0 is carrier on), and a stop bit
  (carrier off). This matches the output of a standard IR receiver module.
- **Receive.** The receiver output goes to RX1. USART1 is set to 2400 baud,
  8N1, with its receive-complete interrupt enabled.
- **Message format.** `[source address][destination address][text...]['\0'][checksum]`.
  The checksum is the 8-bit sum of the address bytes and the text. The
  receive interrupt flags a new message only if the checksum matches.
  The source address is `0x01` and the destination is `0xFF`; both are
  constants in the sketch.

## Using it

Serial monitor at 9600 baud:

- `m`: type a message (up to 28 characters), end with Enter.
- `S`: send it over IR.
- `R`: print the last received message, its addresses and both checksums.

## Files

`src/ir_link/ir_link.ino` (about 240 lines).

## Hardware

Two ATmega2560 boards (or one, looped back), an IR LED on pin 45 and a
38 kHz IR receiver module on RX1 (pin 19).
