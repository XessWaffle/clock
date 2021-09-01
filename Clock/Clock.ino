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

#include "Digit.h"

#include <Wire.h>
#include <ds3231.h>
 
struct ts t;

typedef struct Timer{
  int hour = 0, minu = 0, sec = 0, ms = 0;
  bool on = false;
} timer_t;

const int PWR[3] = {7,8,9};
const int MODE[3] = {4,5,6};
Digit disp[DIGITS];
timer_t timer;

int sec = 0;
int mode = CLOCK_MODE;
int brightness = MAX_BRIGHTNESS;

bool off = false;
bool setup_flag = false;

void setup() {
  // put your setup code here, to run once
  
  noInterrupts();

  for(int i = 0; i < PWR_PINS; i++){
    pinMode(PWR[i], OUTPUT);
    digitalWrite(PWR[i], HIGH); 
  } 

  for(int i = 0; i < MODE_PINS; i++){
    pinMode(MODE[i], OUTPUT);
    digitalWrite(MODE[i], LOW);
  }

  digitalWrite(MODE[0], HIGH);

  pinMode(SER, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(BE, OUTPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SELECT, INPUT);

  digitalWrite(BE, HIGH);

  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 10000;
  OCR1B = MAX_BRIGHTNESS;
  TCCR1B |= (1 << CS11) | (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);

  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  OCR2A = 249;

  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);
  
  TIMSK2 |= (1 << OCIE2A);

  for(int i = 0; i < DIGITS; i++){
    disp[i] = Digit();
  }
  
  interrupts();

  playStartup();
  
}

ISR(TIMER2_COMPA_vect){
  if(mode == TIMER_MODE && !setup_flag){
    if(timer.hour == 0){
      setDoubleDigits(timer.minu, timer.sec, timer.ms/10);
    } else {
      setDoubleDigits(timer.hour, timer.minu, timer.sec);
    }

    if(timer.on){
      if(timer.ms > 0) {
        timer.ms--;
      } else {
        timer.ms = 1000;
        if(timer.sec > 0){
          timer.sec--;
        } else {
          timer.sec = 59;
          if(timer.minu > 0) {
            timer.minu--;
          } else {
            timer.minu = 59;
            if(timer.hour > 0) timer.hour--;
          }
        }
      }
  
      if(timer.hour == 0 && timer.minu == 0 && timer.sec == 0 && timer.ms == 0){
        timer.on = false;
      }    
    }
  }
}

ISR(TIMER1_COMPA_vect){
  digitalWrite(PWR[sec++], HIGH);
  if(sec >= PWR_PINS) sec = 0;

  for(int i = 0; i < 16; i++){
    shift();
  }
}

ISR(TIMER1_COMPB_vect){

  digitalWrite(OE, HIGH);

  int r = 2 * sec;
  int l = 2 * sec + 1;

  char displ = disp[l].convert();
  char dispr = disp[r].convert();
  
  for(int i = 0; i < 8; i++){
    digitalWrite(SER, (displ >> i) & 0x01);
    shift();
  }
  
  for(int i = 0; i < 8; i++){
    digitalWrite(SER, (dispr >> i) & 0x01);
    shift();  
  }
  
  digitalWrite(SER, LOW);
  shift();
  
  digitalWrite(OE, LOW);
  
  digitalWrite(PWR[sec], LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!off){  
     if(mode == CLOCK_MODE){

        if(setup_flag){
          setup_clock();
        }

        digitalWrite(BE, HIGH);
        
        getTime();
        setClockDigits();
     } else if(mode == TIMER_MODE){

        if(setup_flag){
          setup_timer();
        }
        
     } else if(mode == DATE_MODE){
        
        if(setup_flag){
          setup_date();
        }

        digitalWrite(BE, LOW);
        
        getTime();
        setDateDigits();
     }
  }

  int pos = analogRead(A0);
  int select = digitalRead(SELECT);
  int start = digitalRead(LEFT);
  
  if(pos < OFF_THRESHOLD){
    if(!off){
      int check = map(pos, 0, 1023, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
      
      if(check != brightness){
        brightness = check;
        OCR1B = check;  
      }
    } else {
      interrupts();
      off = false;
    }
  } else {
    noInterrupts();
    turnOff();
  }

  if(select){
    delay(TRANSITION_DELAY);
    if(!digitalRead(SELECT)){
      switchMode(); 
    } else {
      setup_flag = true;
    }
    
  }

  if(start && mode == TIMER_MODE){

    delay(TRANSITION_DELAY);
    
    if(timer.on) {
      stopTimer();
    } else {
      startTimer();
    }
  }
}

void playStartup(){
  char debug = 1;

  digitalWrite(BE, LOW);

  for(int i = 0; i < DIGITS; i++){
    for(int j = 0; j < 7; j++){
      disp[i].custom(debug << j);
      delay(100);
    }
  }

  for(int i = 0; i < 7; i++){
    for(int j = 0; j < DIGITS; j++){
      debug = debug | (1 << i);
      disp[j].custom(debug);
    }
    delay(250);
  }
  
  
}

void switchMode(){
  if(mode == CLOCK_MODE){
    mode = TIMER_MODE;    
  } else if(mode == TIMER_MODE){
    mode = DATE_MODE;
  } else if(mode == DATE_MODE){
    mode = CLOCK_MODE;
  }

  dispMode(mode);
  
}

void setDoubleDigits(char a, char b, char c){
  setDigits((a/10), (a % 10), (b/10), (b % 10), (c/10) % 10, (c % 10));
}

void setClockDigits(){
  setDigits((t.hour/10), (t.hour % 10), (t.min/10), (t.min % 10), (t.sec/10), (t.sec % 10));
}

void setDateDigits(){
  setDigits((t.mon/10), (t.mon % 10), (t.mday/10), (t.mday % 10), (t.year/10) % 10, (t.year % 10));
}

void setTimerDigits(){
  setDoubleDigits(timer.hour, timer.minu, timer.sec);
}

void setDigits(char a, char b, char c, char d, char e, char f){
  disp[5].setNumber(a);
  disp[4].setNumber(b);
  disp[3].setNumber(c);
  disp[2].setNumber(d);
  disp[1].setNumber(e);
  disp[0].setNumber(f);

}

void startTimer(){
  setDigits(0,3,0,3,0,3);
  //delay(1000);
  setDigits(0,2,0,2,0,2);
  //delay(1000);
  setDigits(0,1,0,1,0,1);
  //delay(1000);
  timer.on = true;
}

void stopTimer(){
  timer.on = false;
}

void getTime(){
  Serial.begin(9600);
  Wire.begin();
  DS3231_get(&t);
  Serial.end();
  Wire.end();

  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SELECT, INPUT);
}

void setTime(){
  Serial.begin(9600);
  Wire.begin(); 
  DS3231_set(t);
  Wire.end();
  Serial.end();

  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SELECT, INPUT);
}

void setup_date(){
  bool months = false;
  bool days = false;
  bool years = true;

  //getTime();
  int year = 0;
  int mon = 0;
  int day = 0;
  
  setDigits(mon/10,mon%10,day/10,day%10,year/10,year%10);
  delay(1000);

  /*while(true){
    int select = digitalRead(SELECT);
    int right = digitalRead(RIGHT);
    int left = digitalRead(LEFT);

    setDigits(0, left, 0, select, 0, right);

    
  }*/

  
  while(setup_flag){
    while(days){
      day = map(analogRead(A0), 0, 1023, 1, 31);
      setDigits(mon/10,mon%10,day/10,day%10,year/10,year%10);
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        days = false;
        months = true;
        years = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        days = false;
        years = false;
        months = false;
        break;
      }
    }

    while(months){
      mon = map(analogRead(A0), 0, 1023, 1, 12);
      setDigits(mon/10,mon%10,day/10,day%10,year/10,year%10);
    
    /*if(digitalRead(RIGHT) == HIGH){
      seconds = false;
      hours = true;
      minutes = false;
      delay(500);
      break;
    }*/

      if(digitalRead(LEFT) == HIGH){
        days = false;
        months = false;
        years = true;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        days = false;
        years = false;
        months = false;
        break;
      }
    }

    while(years){
      year = map(analogRead(A0), 0, 1023, 0, 99);
      setDigits(mon/10,mon%10,day/10,day%10,year/10,year%10);
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        days = true;
        months = false;
        years = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        days = false;
        years = false;
        months = false;
        break;
      }
    }
    
  }

  getTime();
  t.mday = day;
  t.mon = mon;
  t.year = 2000 + year;
  setTime();
  delay(1000);
}

void setup_clock(){
  bool seconds = false;
  bool minutes = false;
  bool hours = true;

  getTime();

  t.hour=0; 
  t.min=0;
  t.sec=0;

  setClockDigits();
  delay(1000);

  /*while(true){
    int select = digitalRead(SELECT);
    int right = digitalRead(RIGHT);
    int left = digitalRead(LEFT);

    setDigits(0, left, 0, select, 0, right);

    
  }*/

  
  while(setup_flag){
    while(seconds){
      t.sec = map(analogRead(A0), 0, 1023, 0, 60);
      setClockDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }

      
    }
    
    while(minutes){
      t.min = map(analogRead(A0), 0, 1023, 0, 60);
      setClockDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = true;
        hours = false;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = true;
        hours = false;
        minutes = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }
    }
    
    while(hours){
      t.hour = map(analogRead(A0), 0, 1023, 0, 24);
      setClockDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = false;
        minutes = true;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = false;
        hours = false;
        minutes = true;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }
    }
  }
  
  setTime();
  delay(1000);
}

void setup_timer(){
  bool seconds = false;
  bool minutes = false;
  bool hours = true;

  timer.hour=0; 
  timer.minu=0;
  timer.sec=0;

  setTimerDigits();
  delay(1000);

  /*while(true){
    int select = digitalRead(SELECT);
    int right = digitalRead(RIGHT);
    int left = digitalRead(LEFT);

    setDigits(0, left, 0, select, 0, right);

    
  }*/

  
  while(setup_flag){
    while(seconds){
      timer.sec = map(analogRead(A0), 0, 1023, 0, 60);
      setTimerDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = false;
        hours = true;
        minutes = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }

      
    }
    
    while(minutes){
      timer.minu = map(analogRead(A0), 0, 1023, 0, 60);
      setTimerDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = true;
        hours = false;
        minutes = false;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = true;
        hours = false;
        minutes = false;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }
    }
    
    while(hours){
      timer.hour = map(analogRead(A0), 0, 1023, 0, 24);
      setTimerDigits();
      
      /*if(digitalRead(RIGHT) == HIGH){
        seconds = false;
        hours = false;
        minutes = true;
        delay(500);
        break;
      }*/

      if(digitalRead(LEFT) == HIGH){
        seconds = false;
        hours = false;
        minutes = true;
        delay(500);
        break;
      }

      if(digitalRead(SELECT) == HIGH){
        setup_flag = false;
        seconds = false;
        hours = false;
        minutes = false;
        break;
      }
    }
  }
  
  delay(1000);
  startTimer();
}

void dispMode(int modenum){
  for(int i = 0; i < MODE_PINS; i++){
    digitalWrite(MODE[i], LOW);
  }

  switch (modenum){

    case 1:
      digitalWrite(MODE[2], HIGH);
      break;
    case 2:
      digitalWrite(MODE[1], HIGH);
      break;
    case 3:
      digitalWrite(MODE[1], HIGH);
      digitalWrite(MODE[2], HIGH);
      break;
    case 4:
      digitalWrite(MODE[0], HIGH);
      break;
    case 5:
      digitalWrite(MODE[0], HIGH);
      digitalWrite(MODE[2], HIGH);
      break;
    case 6:
      digitalWrite(MODE[0], HIGH);
      digitalWrite(MODE[1], HIGH);
      break;
    case 7:
      digitalWrite(MODE[0], HIGH);
      digitalWrite(MODE[1], HIGH);
      digitalWrite(MODE[2], HIGH);
      break;
  }
}

void turnOff(){
  for(int i = 0; i < PWR_PINS; i++){
    digitalWrite(PWR[i], HIGH);
  }
  off = true;
}

void shift(){
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
}
