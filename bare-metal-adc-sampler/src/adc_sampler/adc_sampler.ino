#define TEST_PIN 14
#define SAMPLE_PERIOD 25                          // in terms of microseconds
#define SAMPLE_FREQUENCY 1000000/SAMPLE_PERIOD       // 1/SAMPLE_PERIOD 
#define ISR_COUNTS 16*SAMPLE_PERIOD  // 25us * 16M/s * 10^-6
#define NUM_SAMPLES 256

volatile uint8_t adc_reading[NUM_SAMPLES];
volatile bool sampling_complete = false;
volatile bool NEW_SAMPLE = false;
volatile bool fillBuffer = false;
volatile uint8_t upperThreshold = 160 + 10;
volatile uint8_t lowerThreshold = 160 - 10;
char buff[256]; // Increased buffer size to avoid overflow

void setup() {
  pinMode(TEST_PIN, OUTPUT);
  Serial.begin(9600);
  // Configure ADC: A0, AVCC reference, left-justified result
  ADMUX = (1 << REFS0)| (1<<ADLAR); 
  // Configure ADC: Enable, Prescaler=16M/(sampling rate x 13), 10-bit conversion takes about 13 cycles
  // Prescaler 16 or less should be fine
  ADCSRA = (1 << ADEN) | (1 << ADPS2);
  // Enable ADC Interrupt
  ADCSRA |= (1 << ADIE);
  // Configure Timer1 for CTC mode and interrupt
  cli(); // Disable interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  // Set to CTC mode (WGM12), no prescaler (CS10)
  TCCR1B |= (1 << WGM12) | (1 << CS10);
  // Set OCR1A for 25µs (16MHz * 25µs - 1 = 400 - 1 = 399)
  OCR1A = 399;
  // Enable Timer1 compare interrupt A
  TIMSK1 |= (1 << OCIE1A);
  sei(); // Re-enable interrupts
}

void loop() {
uint16_t i;
char cmd;
uint16_t crossCount;
uint8_t cross[NUM_SAMPLES >> 3];
uint16_t frequency, period;
uint16_t sum;
        if (Serial.available()) { 
            cmd = Serial.read();
            switch (cmd) { 
                case 'f':
                    fillBuffer = true;
                    while (fillBuffer == true);
                    // Print them out for verification
                    sprintf(buff, "The last %d ADC samples from the microphone are:", NUM_SAMPLES);
                    Serial.println(buff);
                    for (i = 0; i < NUM_SAMPLES; i++) { // print-out samples
                        if ((i & 0x0F) == 0){                         
                          sprintf(buff,"\r\nS[%2d]", i); 
                          Serial.print(buff);
                        }
                        sprintf(buff, "%3d ", adc_reading[i]);
                        Serial.print(buff);
                    }
                    Serial.println("");
                    // Look for positive going "zero" crosses
                    crossCount = 0;
                    for (i = 0; i < NUM_SAMPLES - 1; i++) {
                        if ((adc_reading[i] <= 160) && (adc_reading[i + 1] > 160)) {
                            cross[crossCount++] = i;
                        }
                    }
                    Serial.println("The sound wave crossed at the following indecies");
                    for (i = 0; i < crossCount; i++){
                        sprintf(buff, "%2d ", cross[i]);
                        Serial.print(buff);
                    }           
                    Serial.println("");
                    // print out wave periods and calculate the sum of the periods
                    sum = 0;
                    sprintf(buff, "The sound wave had %d periods", crossCount - 1);
                    Serial.println(buff);
                    if (crossCount>=2){
                      for (i = 0; i < crossCount - 1; i++) {
                          sprintf(buff, "%2d - %2d = %2d", cross[i + 1], cross[i], cross[i + 1] - cross[i]);
                          Serial.println(buff);
                          sum += cross[i + 1] - cross[i];
                      }
                      // Calculate the period and frequency based on the average period
                      period = (SAMPLE_PERIOD * sum) / (crossCount - 1); // every two points is 25 us, times how many points is the total time in a period
                      frequency = ((crossCount - 1) * SAMPLE_FREQUENCY) / sum;
                      sprintf(buff, "average period = %u us", period);
                      Serial.println(buff);
                      sprintf(buff, "average frequency = %d Hz", frequency);
                      Serial.println(buff);
                    }
                    break; 
                default:
                    sprintf(buff, "Unknown key %c", cmd);
                    Serial.println(buff);
                    break;
            } 
}
}

// Timer1 Compare Match A Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
  digitalWrite(TEST_PIN, HIGH);
  // Start the ADC conversion
  ADCSRA |= (1 << ADSC);
  digitalWrite(TEST_PIN, LOW);
}

typedef enum {
    MIC_IDLE, MIC_WAIT_FOR_TRIGGER, MIC_ACQUIRE
} myADCstates_t;

// ADC Conversion Complete Interrupt Service Routine
ISR(ADC_vect) {
//  uint8_t low_byte = ADCL; // The Arduino ADC requires ADCL to be read first if you need a 10-bit precision
  uint8_t high_byte = ADCH; 
  static myADCstates_t myADCstate = MIC_IDLE;
  static uint16_t index = 0;
  switch (myADCstate) { // and do what it tells you to do
        case MIC_IDLE:
            if (fillBuffer == true) {
                myADCstate = MIC_WAIT_FOR_TRIGGER;
            }
            break;
        case MIC_WAIT_FOR_TRIGGER:               
                if ((high_byte > upperThreshold) || (high_byte < lowerThreshold)) {
                    index = 0;
                    myADCstate = MIC_ACQUIRE;
                }           
            break;
        case MIC_ACQUIRE:
            adc_reading[index++] = high_byte;
            if (index == NUM_SAMPLES) {
                fillBuffer = false;
                index = 0;
                myADCstate = MIC_IDLE;
            }
            break;
        default:
            myADCstate = MIC_IDLE;
            break;
    }
}
