#define SPEAKER_PIN 5   
#define TEST_PIN 13     
#define SONG_LENGTH 3

volatile bool playNote = false;
volatile bool incNote = false;
volatile bool doSomethingBad = false;
volatile bool printFlag = false;

uint16_t halfPeriod[SONG_LENGTH] = {8112, 9088, 10208};  // ~A4, G#4, G4
volatile uint8_t noteIndex = 0;
char buff[20];

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(TEST_PIN, OUTPUT);
  Serial.begin(9600);
  milliSecondDelay(500); 

  Serial.println("Demo");
  Serial.println("Interrupt Music Box");
  Serial.println("Skyboard");
  Serial.println("Pin 5 -> Speaker Output");
  Serial.println("Pin 13 -> Test PIN");
  Serial.print("> ");

// Disable global interrupts temporarily while configuring the timer
  cli();
  TCCR1A = 0;              // Normal mode          
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);   // No prescaler
// Enable Timer1 Overflow Interrupt
  TIMSK1 |= (1 << TOIE1); // Set TOIE1 bit to enable overflow interrupt
// Enable global interrupts
  sei();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case '?':
        Serial.println("------------------------------");
        Serial.print("TMR1 = ");
        Serial.println(TCNT1);
        Serial.println("------------------------------");
        Serial.println("?: Help menu");
        Serial.println("z: Clear the terminal");
        Serial.println("n/N: play/Stop playing a note");
        Serial.println("i: increment note in song.");
        Serial.println("------------------------------");
        break;

      case 'n':
        Serial.println("Start playing note.");
        playNote = true;
        break;

      case 'N':
        Serial.println("Stop playing note.");
        playNote = false;
        break;

      case 'i':
        Serial.println("Playing next note.");
        incNote = true;
        break;

      case 'z':
        for (int i = 0; i < 40; i++) Serial.println();
        break;

      default:
        sprintf(buff,"%c",cmd);
        Serial.print("Unknown Key: ");
        Serial.println(buff);
        break;
    }
    Serial.print("> ");
  }
}

// Timer1 ISR
ISR(TIMER1_OVF_vect) {
  digitalWrite(TEST_PIN, HIGH);
  if (incNote) {
    noteIndex++;
    if (noteIndex >= SONG_LENGTH) noteIndex = 0;
    incNote = false;
  }
  if (playNote) {
    TCNT1 = 0x10000 - halfPeriod[noteIndex]; 
    digitalWrite(SPEAKER_PIN, !digitalRead(SPEAKER_PIN)); 
  }
  digitalWrite(TEST_PIN, LOW);
}

void milliSecondDelay(uint16_t ms) {
  for (uint16_t i = 0; i < ms; i++) {
    microSecondDelay(1000);
  }
}

void microSecondDelay(uint16_t us) {
  for (uint16_t i = 0; i < us; i++) {
    asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
  }
}