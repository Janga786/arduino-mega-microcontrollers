# ADC Sampler at 40 kHz (register level)

An ATmega2560 sketch that samples analog input A0 at 40 kHz without
`analogRead()`. It was written to measure the frequency of a tone picked up
by the course board's microphone.

## How it works

- `ADMUX` selects A0 with the AVCC reference and a left-adjusted result, so
  the ISR reads only the 8-bit `ADCH`. `ADCSRA` enables the ADC, sets the
  prescaler to 16 (1 MHz ADC clock) and enables its interrupt.
- Timer1 runs in CTC mode with no prescaler and `OCR1A = 399`, so its
  compare interrupt fires every 25 µs and starts one conversion.
- The ADC interrupt is a small state machine (idle, wait for trigger,
  acquire). After the main loop asks for data, it waits until a sample
  leaves the band 150 to 170, then stores the next 256 samples.
- Send `f` over serial (9600 baud). The main loop waits for the buffer,
  prints the 256 samples, finds the upward crossings of the level 160, and
  prints the average period (µs) and frequency (Hz).
- Pin 14 (`TEST_PIN`) pulses inside the Timer1 interrupt so the sample
  timing can be checked on a scope.

## Files

`src/adc_sampler/adc_sampler.ino` (about 145 lines).

## Hardware

An ATmega2560 board with an analog signal on A0 (on the course board, the
microphone).
