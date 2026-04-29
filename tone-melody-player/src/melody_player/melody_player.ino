#define BUTTON_PIN 9
#define SPEAKER_PIN 5

#define NOTE_C5 4780
#define NOTE_D5 4260
#define NOTE_E5 3790
#define NOTE_F5 3580
#define NOTE_G5 3190
#define NOTE_A5 5680
#define NOTE_AS5 5360 
#define NOTE_C6 2390
#define REST 0

#define EIGHTH 150
#define QUARTER 300
#define DOTTED_QUARTER 450
#define HALF 600

#define SONG_LENGTH 25
int melody[] = {
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_F5, NOTE_E5,
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_G5, NOTE_F5,
  NOTE_C5, NOTE_C5, NOTE_C6, NOTE_A5, NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_AS5, NOTE_AS5, NOTE_A5, NOTE_F5, NOTE_G5, NOTE_F5
};
int rhythm[] = {
  EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, HALF,
  EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, HALF,
  EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
  EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, HALF
};

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {

  if (digitalRead(BUTTON_PIN) == LOW) {

    for (int i = 0; i < SONG_LENGTH; i++) {
      playNote(melody[i], rhythm[i]);
    }

    while(digitalRead(BUTTON_PIN) == LOW);

  }
}

void playNote(int note, int duration) {
  if (note == REST) {
    delay(duration);

  } else {

    long cycles = (long)duration * 1000L * 16 / note / 2;
    for (long i = 0; i < cycles; i++) {

      digitalWrite(SPEAKER_PIN, HIGH);
      microSecondDelay(note / 16);
      digitalWrite(SPEAKER_PIN, LOW);
      microSecondDelay(note / 16);

    }
  }
}

void microSecondDelay(uint16_t us) {
  if (us <= 2) return;

  for (uint16_t i = 0; i < us; i++) {

    asm volatile("nop"); 
    asm volatile("nop"); 
    asm volatile("nop");
    asm volatile("nop"); 
    asm volatile("nop"); 
    asm volatile("nop");
    asm volatile("nop"); 
    asm volatile("nop"); 
    asm volatile("nop");

  }
}