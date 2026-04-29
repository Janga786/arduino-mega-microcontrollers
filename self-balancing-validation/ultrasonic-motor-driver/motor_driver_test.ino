const int pwm = 3;
const int Ain1 = 35;
const int Ain2 = 34;
const int standby = 36;

void setup() {
  pinMode(pwm, OUTPUT);
  pinMode(Ain1, OUTPUT);
  pinMode(Ain2, OUTPUT);
  pinMode(standby, OUTPUT);
}

void loop() {
  digitalWrite(Ain1, HIGH);
  digitalWrite(Ain2, LOW);
  digitalWrite(standby,HIGH);
  analogWrite(pwm, 128);
}