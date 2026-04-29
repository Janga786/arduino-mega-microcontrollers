#define LED_ON 42
#define IR_LED_PIN 45
#define MAX_BUFFER_SIZE 32

volatile bool transmitStart = false;
volatile bool receiveNewMessage = false;

uint8_t transmitSourceAddr = 0x01;
uint8_t transmitDestinationAddr = 0xFF;

volatile uint8_t transmitChecksum = 0;
volatile uint8_t receiveChecksum  = 0;

volatile uint8_t receiveSourceAddr = 0;
volatile uint8_t receiveDestinationAddr = 0;

char transmitIRBuffer[MAX_BUFFER_SIZE];
char receiveIRBuffer[MAX_BUFFER_SIZE];

uint8_t  baudRateSelected = 2;
uint16_t bitPeriod[6] = {53333,13333,6666,3333,1666,833};

char buff[96];
char cmd;
int32_t i;

#define BAUD_VALUE 416

void setup() {
  pinMode(IR_LED_PIN, OUTPUT);
  Serial.begin(9600);

  UBRR1H = (BAUD_VALUE >> 8);
  UBRR1L = (uint8_t)BAUD_VALUE;
  // Enable receiver, transmitter, and RX Complete interrupt
  UCSR1B = (1 << RXEN1) | (1 << RXCIE1);
  // Set frame format: 8 data bits, no parity, 1 stop bit
  UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);

  TCCR5A = (1 << WGM51);
  TCCR5B = (1 << WGM52) | (1 << WGM53) | (1 << CS50);
  ICR5   = 420;
  OCR5B  = LED_ON;
  TCCR5A &= ~(1 << COM5B1);

  TCCR3A = 0x00;
  TCCR3B = 0x00;
  TCNT3  = 0x10000 - bitPeriod[baudRateSelected];
  TIFR3 |= (1 << TOV3);
  TIMSK3 |= (1 << TOIE3);
  TCCR3B |= (1 << CS30);

  asm volatile ("sei");
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
    switch (cmd) {
      case 'm':
          Serial.println("Enter the message, hit return when done.");
          Serial.print(">");
          transmitIRBuffer[0] = transmitSourceAddr;      
          transmitChecksum = transmitSourceAddr;                
          transmitIRBuffer[1] = transmitDestinationAddr; // Generally unprintable
          transmitChecksum += transmitDestinationAddr;
          while(Serial.available()==0);
          i=2;
          while ( ((cmd = Serial.read()) != '\r') && (i < MAX_BUFFER_SIZE-2)) {
              sprintf(buff,"%c",cmd);
              Serial.print(buff);
              transmitIRBuffer[i++] = cmd;
              transmitChecksum += cmd;
              if  (i >= MAX_BUFFER_SIZE-3) {
                  sprintf(buff,"maximum message length of %d characters reached\r\n",MAX_BUFFER_SIZE-4);
                  Serial.println(buff);
              }
              while(Serial.available()==0);                                
          }              
          transmitIRBuffer[i] = '\0';                     // Null terminate string
          transmitIRBuffer[i+1] = transmitChecksum;       // Checksum of the message
          Serial.println("\r\nMessage entered");
          break;

      case 'S':
          transmitStart = true;
          while (transmitStart == true);
          Serial.println("Transmitted");
          sprintf(buff,"Message: %s", transmitIRBuffer+2);// Treat the array name 'transmitIRBuffer' as a pointer points to the first element
          // +2 will point to the first message character in the array, then it directly prints out the string and ends at '\0'.
          Serial.println(buff);
          sprintf(buff,"Checksum computed: %d", transmitChecksum);
          Serial.println(buff);
          sprintf(buff,"Sender address: %d",transmitSourceAddr);
          Serial.println(buff);
          sprintf(buff,"Target address: %d",transmitDestinationAddr);
          Serial.println(buff);
          break;

      case 'R':
          if (receiveNewMessage == true) {                  
              receiveSourceAddr = receiveIRBuffer[0];
              receiveChecksum = receiveIRBuffer[0];                          
              receiveDestinationAddr = receiveIRBuffer[1];
              receiveChecksum += receiveIRBuffer[1];                  
              i = 2;
              while (receiveIRBuffer[i] != '\0') {
                  receiveChecksum += receiveIRBuffer[i];
                  i += 1;
              }
              transmitChecksum = receiveIRBuffer[i+1];                  
              Serial.println("Received");
              sprintf(buff,"Message:            %s",receiveIRBuffer+2);
              Serial.println(buff);
              sprintf(buff,"Checksum computed:  %d",transmitChecksum);
              Serial.println(buff);
              sprintf(buff,"Checksum received:  %d",receiveChecksum);
              Serial.println(buff);
              sprintf(buff,"Sender address:     %d",receiveSourceAddr);
              Serial.println(buff);                   
              sprintf(buff,"Target address:     %d", receiveDestinationAddr);
              Serial.println(buff);                   
              receiveNewMessage = false;
          } else {
              Serial.println("No message, receiveNewMessage = false");
          }
          break;
    }
  }
}

typedef enum  {TX_IDLE, TX_DATA_BITS, TX_STOP_BITS, TX_START_BITS} tmr3ISRstate_t;

ISR(TIMER3_OVF_vect) {
    static uint8_t transmitBufferIndex = 0;
    static tmr3ISRstate_t tmr3ISRstate = TX_IDLE;
    static uint8_t tx_checksum = false;
    static uint8_t mask;
    static char letter;

    switch (tmr3ISRstate) {
        case TX_IDLE:
          if (transmitStart == true) {
            // Your Code//
            transmitBufferIndex = 0;
            tx_checksum = false;
            mask = 0x01;
            tmr3ISRstate = TX_START_BITS;
          }
          break;

        case TX_START_BITS:
          // Your Code//
          TCCR5A |= (1 << COM5B1);
          OCR5B = LED_ON;
          mask = 0x01;
          letter = transmitIRBuffer[transmitBufferIndex];
          tmr3ISRstate = TX_DATA_BITS;
          break;             

        case TX_DATA_BITS:            
          // Your Code//
          if (letter & mask) {
            TCCR5A &= ~(1 << COM5B1);
          } else {
            TCCR5A |= (1 << COM5B1);
            OCR5B = LED_ON;
          }
          mask <<= 1;
          if (mask == 0) {
            tmr3ISRstate = TX_STOP_BITS;
          }
          break;                

        case TX_STOP_BITS:
          // Your Code//
          TCCR5A &= ~(1 << COM5B1);
          if (transmitIRBuffer[transmitBufferIndex] == '\0') {
              tmr3ISRstate = TX_START_BITS;  
              tx_checksum = true;
          } 
          else if (tx_checksum == true) {
              tmr3ISRstate = TX_IDLE;  
              tx_checksum = false;
              transmitStart = false;                
          } 
          else {
              tmr3ISRstate = TX_START_BITS;                  
          }
          transmitBufferIndex += 1;
          break;               

        default:
          tmr3ISRstate = TX_IDLE;
          TCCR5A &= ~(1 << COM5B1);
    }
    // Your Code//; // 2400 Baud
    TCNT3 = 0x10000 - bitPeriod[baudRateSelected];
}

typedef enum  {RX_IDLE, RX_DATA_BYTES, RX_CHECKSUM} myUSART1ISR_t;
//----------------------------------------------
// My EUSART2 ISR to handle incoming characters from IR decoder
//----------------------------------------------
ISR(USART1_RX_vect) {   
  static myUSART1ISR_t usart1ISRstate = RX_IDLE;
  static uint8_t receiveBufferIndex = 0;
  switch(usart1ISRstate) {
      case RX_IDLE:
          receiveBufferIndex = 0;
          receiveIRBuffer[receiveBufferIndex++] =  UDR1;        
          usart1ISRstate = RX_DATA_BYTES;
          break;          
      case RX_DATA_BYTES:            
          if ((receiveIRBuffer[receiveBufferIndex++] =  UDR1) == '\0')    
             usart1ISRstate = RX_CHECKSUM;            
          break;         
      case RX_CHECKSUM:
          // Your Code//
          receiveIRBuffer[receiveBufferIndex++] = UDR1;
          {
            uint8_t j = 0;
            uint8_t s = 0;
            while (j < receiveBufferIndex && receiveIRBuffer[j] != '\0') {
              s += (uint8_t)receiveIRBuffer[j];
              j++;
            }
            uint8_t got = 0;
            if ((j + 1) < receiveBufferIndex) {
              got = (uint8_t)receiveIRBuffer[j + 1];
            }
            receiveNewMessage = (s == got);
          }
          usart1ISRstate = RX_IDLE;
          break;          
      default:
          usart1ISRstate = RX_IDLE;
          break;
  }
}
