# Bare-Metal ADC Sampler at 40 kHz

A direct-register-access ATmega2560 ADC sampler that **does not call
`analogRead()` once**.  The ADC is configured by writing `ADMUX`,
`ADCSRA`, and `ADCSRB` directly; the sample timing is governed by a
Timer1 CTC interrupt; samples land in a 256-entry ring buffer; and a
threshold-based event detector flags peak / trough crossings without
ever blocking the main loop.

The result is a deterministic 40 kHz sample rate (one sample every
25 µs) on a 16 MHz Arduino, with zero jitter.  Sample collection runs
entirely from interrupts so the main loop is free for serial output,
threshold updates, and command parsing.

## What it does

```
   ┌──────────┐         ┌─────────────┐        ┌────────────┐
   │ A0 input │─────────▶│ ATmega2560  │────────▶ Serial print
   │ (analog) │         │   ADC       │        │ (8N1, 9600) │
   └──────────┘         │             │        └────────────┘
                        │  ┌────────┐  │
                        │  │ Timer1 │  │
                        │  │ CTC    │  │ ──── triggers ADC start every 25 µs
                        │  │ ISR    │  │
                        │  └────────┘  │
                        │              │
                        │  ┌────────┐  │
                        │  │ ADC    │  │
                        │  │ ISR    │  │ ──── pushes sample into ring buffer,
                        │  │        │  │      checks against thresholds,
                        │  │        │  │      sets `NEW_SAMPLE` flag
                        │  └────────┘  │
                        └─────────────┘
```

A typical session: drive any analog signal (signal generator, sensor,
or just a finger on the pin) into A0 and watch the buffer fill.  The
serial console reports the rolling min/max, the current threshold
band, and any threshold-crossing events as they occur.

## What's interesting

This sketch demonstrates four things you can't show with vanilla
`analogRead()`:

1. **Direct register access.**  Every important configuration —
   reference voltage, input MUX selection, prescaler, left/right
   justification, interrupt enable — is set by toggling the bits of
   `ADMUX` and `ADCSRA` by hand.  Code looks like:

   ```c
   // Configure ADC: A0, AVCC reference, left-justified result
   ADMUX  = (1 << REFS0) | (1 << ADLAR);
   // Enable, prescaler = 16 (≈ 1 MHz ADC clock), interrupt enable
   ADCSRA = (1 << ADEN)  | (1 << ADPS2) | (1 << ADIE);
   ```

2. **Hard-real-time interrupt timing.**  Timer1 is set up in CTC
   mode with `OCR1A = 16 MHz × 25 µs = 400`, and its compare-match
   interrupt is the *only* trigger for an ADC conversion.  The
   sample period is therefore exactly 25 µs to the resolution of the
   16 MHz crystal — no `millis()` jitter, no `delay()` skew.

3. **Lock-free producer/consumer.**  The ADC ISR is the producer;
   the main loop is the consumer.  Synchronisation is just the
   `volatile` qualifier on the shared flag — no `cli()` /`sei()`
   pairs in the main loop because the ISR only writes the flag and
   the main loop only reads-and-clears it.

4. **In-line peak/trough detection.**  Two volatile thresholds
   (`upperThreshold`, `lowerThreshold`) gate sample storage so the
   buffer only fills when the signal is in the "interesting" band.
   The thresholds are runtime-mutable from the serial console.

## What's in this folder

```
bare-metal-adc-sampler/
└── src/
    └── adc_sampler/
        └── adc_sampler.ino        # ~145 lines, fully commented
```

## Hardware

- An Arduino Mega 2560 (or compatible ATmega2560 board).
- An analog signal source on A0.  A potentiometer wiper is fine for
  a smoke test; a function generator is better for stress-testing
  the ISR timing.
- Pin 14 (`TEST_PIN`) is toggled at the ADC ISR rate so you can
  scope-verify the sample timing on a logic analyser.
