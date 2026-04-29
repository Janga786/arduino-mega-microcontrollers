The music game
#define SPEAKER_PIN 5
#define BUTTON_PIN 9 // The bottom button
char buff[20];

// Note definitions
#define Ds4     0
#define E4      1
#define F4      2
#define Fs4     3
#define G4      4
#define Gs4     5
#define A5      6
#define B5      8
#define C5      10
#define E5      12
#define F5      13
#define G5      15

// Duration definitions (Timer3 counts)
#define WHO     62500
#define HAL     31250
#define QUA     15625
#define EIG     7813
#define REST    1953
#define DELTA   10000

// Song configuration
#define NUM_OCTAVES 2
#define MAX_SONG_LENGTH 17
#define NUM_SONGS 4

// Song indices
#define TEST_SONG 0
#define COPY_SONG 1
#define FAIL_SONG 2
#define SUCCESS_SONG 3

volatile bool playingSong = false;
volatile uint8_t noteIndex = 0;
volatile bool playNote = false;
volatile uint8_t songNumber = 0;
volatile bool success = true;
volatile uint16_t halfPeriod = 0;

uint16_t scale[NUM_OCTAVES*12] = {
  12864,12144,11472,10816,10208,9632,9088,8592,8112,7648,7232,6816,
  6432,6080,5728,5408,5104,4816,4560,4288,4048,3824,3616,3408
};

uint8_t notes[NUM_SONGS][MAX_SONG_LENGTH] = {
  {B5, A5, G4, B5, A5, G4, G4, G4, G4, G4, A5, A5, A5, A5, B5, A5, G4},
  {B5, A5, A5, B5, A5, A5},
  {C5, Gs4},
  {C5, F5, F5}
};

uint16_t duration[NUM_SONGS][MAX_SONG_LENGTH] = {
  {QUA, QUA, HAL, QUA, QUA, HAL, EIG, EIG, EIG, EIG, EIG, EIG, EIG, EIG, QUA, QUA, HAL},
  {HAL, HAL, HAL},
  {HAL, WHO},
  {WHO, QUA, QUA}
};
uint8_t songLength[NUM_SONGS] = {17, 3, 2, 3};

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  milliSecondDelay(500); 

  Serial.println("\n*** Arduino Music Box ***");
  Serial.println("Install two jumpers: Pin 5 (PE3) connected to LPF input, LPF output conected to PRE");
  Serial.println("------------------------------");
  Serial.println("?: Help menu\nz: Clear terminal\np: play song once");
  Serial.println("------------------------------");
  Serial.print("> ");

// Disable global interrupts temporarily while configuring the timer
  cli();
  TCCR1A = 0;              // Normal mode          
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);   // 1:1 prescaler

  TCCR3A = 0;              // Normal mode          
  TCCR3B = 0;
  TCCR3B |= (1 << CS32);   // 1:256 prescaler 
// Enable TIMER1 and TIMER3 Overflow Interrupt
  TIMSK1 |= (1 << TOIE1); // Set TOIE1 bit to enable overflow interrupt
  TIMSK3 |= (1 << TOIE3); // Set TOIE3 bit to enable overflow interrupt
// Enable global interrupts
  sei();
}

void loop() {
uint8_t i;
char cmd;
uint16_t pressDuration, prevPressTMR;
uint8_t success = true;

  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case '?':
        Serial.println("------------------------------");
        Serial.println("?: Help menu\nz: Clear terminal\np: Play song once");
        Serial.println("------------------------------");
        break;
      case 'z':
        for (int i = 0; i < 40; i++) Serial.println();
        break;
      case 'p':
        songNumber = TEST_SONG;
        noteIndex = 0;
        playingSong = true;
        break;
      case 'r':
        Serial.println("Listen to the beat pattern.");
        songNumber = COPY_SONG;
        noteIndex = 0;
        playingSong = true;
        while(playingSong == true);

        
        TIMSK3 &= ~(1 << TOIE3);

        Serial.println("Use the bottom button to reproduce this pattern.");
        while(digitalRead(BUTTON_PIN)==HIGH);
        playingSong = false;
        noteIndex = 0;
        success = true;

        while ((noteIndex < songLength[COPY_SONG]) && (success == true)){
              // "positive" edge of button press
              if (noteIndex != 0) {
                  pressDuration = TCNT3 - prevPressTMR;// TCNT3 keeps counting as time elapses
                  prevPressTMR = TCNT3;
                  if ( (pressDuration < (duration[songNumber][noteIndex-1] + REST - DELTA)) ||
                       (pressDuration > (duration[songNumber][noteIndex-1] + REST + DELTA))) success = false;                                                                                        
              } else {
                  prevPressTMR = TCNT3; // if noteIndex is 0, it starts from here.
              }
              
              // Your Code //                        // play the note while
              halfPeriod = scale[notes[COPY_SONG][noteIndex]];
              playNote = true;
              while(digitalRead(BUTTON_PIN)==LOW);
              // Your Code //      // button is being held down
              
              playNote = false;    // release the button to stop playing the note                   
              if (noteIndex < songLength[COPY_SONG] - 1) {                            
                  // Your Code // // stay here until the next press
                  while(digitalRead(BUTTON_PIN)==HIGH);
              }
              noteIndex += 1;            
          }
  // get out of the while loop when it reaches the end of the copy song

TIMSK3 |= (1 << TOIE3);
if (success == true) {
            songNumber = SUCCESS_SONG;
            noteIndex = 0;
            playingSong = true;
            Serial.println("Winner!");
        } 
else {
            songNumber = FAIL_SONG;
            noteIndex = 0;
            playingSong = true;
            Serial.println("You lose");
        }
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

// TIMER1 ISR
ISR(TIMER1_OVF_vect){
    if(playNote==true){ // Inside here, you need to reset the TCNT1 value and toggle the speaker pin

      TCNT1 = 65536 - halfPeriod;

      PINE = (1 << PE3);

      }
  }

typedef enum  {TMR3_IDLE, TMR3_PLAY_NOTE, TMR3_PLAY_REST} tmr3ISRstate_t;

ISR(TIMER3_OVF_vect){
    static tmr3ISRstate_t tmr3state = TMR3_IDLE;
    switch(tmr3state) {
        case TMR3_IDLE:
            if (playingSong == true) { // Inside this, move the state out of IDLE the next time it enters ISR
              tmr3state = TMR3_PLAY_NOTE;
              noteIndex = 0;
            }
            break;                       
        case TMR3_PLAY_NOTE:
          playNote = true;       
          halfPeriod = scale[notes[songNumber][noteIndex]];
          TCNT3 = 65536 - duration[songNumber][noteIndex];
          tmr3state = TMR3_PLAY_REST;
break;           
        case TMR3_PLAY_REST: // Play the rest as a duration, first, set playNote to be false, then set up the 
          playNote = false;
          noteIndex++;

          if (noteIndex >= songLength[songNumber]) {
                playingSong = false;
                tmr3state = TMR3_IDLE;
                noteIndex = 0;
            }
          else {
                TCNT3 = 65536 - REST;
                tmr3state = TMR3_PLAY_NOTE;
            }
            break;
    }        
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
