#define SER 13  
#define CLK 12
#define OE 11
#define BE 10

#define LEFT 3
#define SELECT 2
#define RIGHT 1

#define PWR_PINS 3
#define MODE_PINS 3
#define GND_PINS 16
#define DIGITS 6

#define CLOCK_MODE 1
#define TIMER_MODE 2
#define DATE_MODE 3

#define MIN_BRIGHTNESS 8000
#define MAX_BRIGHTNESS 2000

#define OFF_THRESHOLD 1000
#define TRANSITION_DELAY 500

const int PWR[3] = {7,8,9};


void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < PWR_PINS; i++){
    pinMode(PWR[i], OUTPUT);
    digitalWrite(PWR[i], HIGH); 
  }

  //Serial.begin(9600);

  pinMode(SER, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(BE, OUTPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SELECT, INPUT);
  pinMode(4, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  //int left = digitalRead(LEFT);
  digitalWrite(4, digitalRead(RIGHT));
  /*Serial.begin(9600);
  Serial.println(right);
  Serial.end();*/
  
  delay(1000);
  
  
  
  
  /*Serial.println(debug);

  digitalWrite(SER, LOW);
  shift();
  shiftVirtual(0);
  delay(1000);

  sprinting(1, 0);
  printReg();

  
  for(int i = 0; i < 16; i++){
    digitalWrite(SER, (debug >> i) & 0x01);
    sprinting(i + 1, (debug >> i) & 0x01);
    shiftVirtual((debug >> i) & 0x01);
    printReg();
    shift();
    delay(100);
  }

  shift();
  
  
  for(int i = 0; i < 8; i++){
    digitalWrite(SER, (debug >> i) & 0x01);
    sprinting(i + 1, (debug >> i) & 0x01);
    shiftVirtual((debug >> i) & 0x01);
    printReg();
    shift();
    delay(100);
    
  }

  digitalWrite(SER, LOW);
  shift();
  shiftVirtual(0);
  delay(100);

  sprinting(9, 0);
  printReg();
  
  for(int i = 0; i < 8; i++){
    digitalWrite(SER, (debug >> i) & 0x01);
    sprinting(i + 10, (debug >> i) & 0x01);
    shiftVirtual((debug >> i) & 0x01);
    printReg();
    shift();  
    delay(100);
  }

  digitalWrite(SER, LOW);
  shift();
  shiftVirtual(0);
  
  debug = debug << 1;

  if(debug >= 1 << 8){
    debug = 1;
  }


  Serial.println("done");
  delay(2500);
  
  for(int i = 0; i < 16; i++){
    shift();
    shiftVirtual(0);
  }*/
   
}

void shift(){
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
}

/*void shiftVirtual(bool highLow){
  for(int i = 14; i >= 0; i--){
    reg[i + 1] = reg[i];
  }

  reg[0] = highLow;
}

void printReg(){
  Serial.print("Virtual Register: [");

  for(int i = 0; i < 16; i++){
    Serial.print(reg[i]);
    if(i < 15)
      Serial.print(", ");
  }

  Serial.println("]");
  
}

void sprinting(int bi, bool hl){
  Serial.print("Shifted Bit: ");
  Serial.print(bi);
  Serial.print("   ");
  Serial.println(hl);
}*/
