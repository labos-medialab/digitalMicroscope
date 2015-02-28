int motorPins[] = {8, 9, 10, 11};
int count = 0;
int count2 = 0;

unsigned int delayTime = 1500;
String data="";

int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;
int val = 0;
int smjer=0;

bool stop=1;

int cancel=0;

void setup() {
  Serial.begin(9600);

  pinMode(encoderPin1, INPUT_PULLUP); 
  pinMode(encoderPin2, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

  for (count = 0; count < 4; count++) {
    pinMode(motorPins[count], OUTPUT);
  }
}

void motorStop(){
  stop=0;
  Serial.println("stop");
  for (count = 3; count >= 0; count--)
    digitalWrite(motorPins[count], LOW);
}

void moveForward() {
  stop=1;
  Serial.println(encoderValue);
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[count], count2>>count&0x01);
  }
  delayMicroseconds(delayTime);
}
void moveBackward() {
  stop=1;
  Serial.println(encoderValue);
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[3 - count], count2>>count&0x01);
  }
  delayMicroseconds(delayTime);
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
    if(encoderValue<-100 && encoderValue>-5000) encoderValue*=1.1;
    else encoderValue-=10;
  }
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011){
    if(encoderValue> 100 && encoderValue<5000) encoderValue*=1.1;
    else encoderValue+=10;
  }
  lastEncoded = encoded;
  if(cancel==200) encoderValue=0;
  cancel=0;
}

void loop() {
  while(encoderValue<0){
    if(cancel < 200) cancel++;
    moveForward();
    encoderValue++;
  }
  while(encoderValue>0){
    if(cancel < 200) cancel++;
    moveBackward();
    encoderValue--;
  }
  if(stop) motorStop();
}

void serialEvent(){
  if(Serial.available()) data=Serial.readStringUntil('\n');
  if(data== "up") smjer=-1;
  else if(data=="down") smjer=1;
  else if(data=="stop"){
    smjer =0;
    motorStop();
  }
  else delayTime=data.toInt();
  Serial.println(smjer);
  Serial.println(delayTime);
}
