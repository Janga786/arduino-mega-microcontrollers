# IR Link-Layer Protocol

A hand-built serial protocol over a 38 kHz IR LED link.  Two ATmega2560
boards run identical firmware: each can act as either a transmitter or
a receiver, and frames carry a source address, a destination address,
a payload, and an XOR checksum.

The whole transport stack — preamble, sync, framing, parity, baud-rate
selection, broadcast-vs-unicast — is implemented by hand without any
infra-red library.  The IR LED is bit-banged from a GPIO and the
receive path is a phototransistor + comparator wired to an ICP pin
that triggers an input-capture interrupt on every transition.

## What it does

```
                 +-------------+              +-------------+
                 │  TX board    │              │  RX board    │
                 │              │              │              │
   user types ──▶│ Serial in    │              │              │
                 │  (8N1, 9600) │              │              │
                 │     │        │              │              │
                 │     ▼        │              │              │
                 │  build frame │              │              │
                 │              │              │              │
                 │  IR LED ─────┼──── 38 kHz ──┼──▶ phototran │
                 │  bit-bang    │   carrier    │   + comparator│
                 │              │   modulated   │     │        │
                 │              │              │     ▼        │
                 │              │              │  ICP edge ISR │
                 │              │              │     │         │
                 │              │              │     ▼         │
                 │              │              │  decode frame │
                 │              │              │     │         │
                 │              │              │     ▼         │
                 │              │              │  validate     │
                 │              │              │  checksum     │
                 │              │              │     │         │
                 │              │              │     ▼         │
                 │              │              │  Serial out ──┼──▶ user reads
                 +-------------+              +-------------+
```

The frame format:

```
 byte 0   : start-of-frame   = 0x7E
 byte 1   : source address
 byte 2   : destination address  (0xFF = broadcast)
 byte 3   : length (N) of payload
 byte 4..4+N-1 : payload bytes
 byte 4+N : XOR checksum of bytes 1..4+N-1
 byte 5+N : end-of-frame     = 0x7F
```

The transmitter walks the frame byte-by-byte, encoding each bit as
either "carrier on for one bit-period" (logic 1) or "carrier off for
one bit-period" (logic 0).  The receiver runs an input-capture ISR
on every IR pulse edge, reconstructs the bit period, and walks the
state machine `IDLE → SOF → SRC → DST → LEN → PAYLOAD → CHECKSUM →
EOF`.  Bad checksums or unexpected EOFs reset the state machine to
`IDLE`.

## Why a hand-rolled protocol?

This isn't NEC, RC-5, RC-6, or any other consumer-IR standard — those
all assume a remote-control use case (short, infrequent button
presses).  This is a **bidirectional point-to-point data link**, more
analogous to a classic RS-232 line, and it teaches the parts of a
real link layer you don't see on a UART: framing, addressing,
broadcast handling, and integrity checking.

The baud-rate selection in particular is what makes the project
worthwhile — there's a runtime-configurable `bitPeriod[]` table that
maps 6 baud rates onto 6 different Timer-OCR values, so you can
trade off between range and throughput at the keyboard.

## What's in this folder

```
ir-link-layer-protocol/
└── src/
    └── ir_link/
        └── ir_link.ino          # ~240 lines; transmitter + receiver
```

The same sketch runs on both boards; the source address (`0x01` /
`0x02`) is set with a compile-time constant.

## Hardware

- 2 × Arduino Mega 2560 (or compatible).
- 1 × IR LED per transmitter (on `IR_LED_PIN`, default pin 45,
  current-limited to ~50 mA peak).
- 1 × IR phototransistor + comparator per receiver (output to the
  ICP-capable pin).
- A line-of-sight optical path between the LED and the phototransistor
  — works reliably to about 1.5 m indoors.

## Things I'd extend it with

- **Forward error correction.**  XOR checksum catches single-bit
  errors but not bit-pair flips — a simple Hamming(7,4) on each
  payload nibble would give one-bit correction at the cost of 75 %
  payload efficiency.
- **Wake-on-preamble.**  Right now the receiver runs the ICP ISR
  full-time.  A 4-byte preamble + a Schmitt-triggered watchdog
  would let the MCU sleep between frames and only wake on the
  first edge of a real packet — important for battery-powered
  remotes.
- **Multi-drop addressing.**  The `0xFF` broadcast is the only
  group address right now; support for 16 group addresses
  (`0xF0..0xFF`) would let you build multi-receiver fan-out at
  zero hardware cost.
