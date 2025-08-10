#include <Arduino.h>
#include <TM1637Display.h>
 
unsigned char state;
enum {set, forward, pace, zig, zag, end};

// Button Pins
const int buttonPinA = 2; // External interrupt 0
const int buttonPinB = 3; // External interrupt 1

// Motor A pins
const int pwma = 10;
const int ain1 = 8;
const int ain2 = 9;

// Motor B pins
const int bin2 = 4;
const int pwmb = 6;
const int bin1 = 7;

// stby pins
const int stby = 11;

// TM1637 display pins
const int CLK = 12;
const int DIO = 13;

// random variables
long rand_move = 0;
long rand_state = 0;

// counter variabless
unsigned int count=0;
unsigned int pace_count=0;

// face button modes
bool wheelMode = false;
bool alarmMode = false;
bool alarm = false;
int k = 750;
bool stop = false;

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);

  // Configure pins as inputs with internal pull-up resistors
  pinMode(buttonPinA, INPUT_PULLUP);
  pinMode(buttonPinB, INPUT_PULLUP);

  // Attach the same ISR to both interrupts
  attachInterrupt(digitalPinToInterrupt(buttonPinA), wheelButtonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPinB), alarmButtonPress, FALLING);

  // Set output pins to motors
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwmb, OUTPUT);

  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwma, OUTPUT);

  randomSeed(analogRead(A0));

  state = set;
}


 /* Finite states that dictate Clocky's motion pattern */
 /* process ends after 45 seconds                      */
void loop() {
  display.showNumberDecEx(k, (0x80 >> 1), true);
  display.setBrightness(0x0f);
  
  switch(state){

    /* this is the setup state, the motion begins when alarm time is reached */
    case set:
      display.showNumberDecEx(k, (0x80 >> 1), true);
      /* clock display functionality */
		  if(k%100 == 59)
			  k += 41;
		  else
			  k += 1;
    /* set k to desired alarm time */
      if(k == 800){
        Serial.println("Alarm sound on");
        alarm = true;
        state = forward;
      }
      else{
        state = set;
        delay(2000);
      }
      break;

    /* clocky motion begins by moving forward for 4 seconds*/
    case forward:
      if(stop){
        state = end;
        stop = false;
        break;
      }
      Serial.println("Move forward");
      digitalWrite(stby, LOW);

      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 180);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 90);
      
      digitalWrite(stby, HIGH);

      /* Move forward for 4 seconds */
      delay(3000);
      count+=4;
      state = pace;
      break;

    /* after moving forward the Clocky paces back and forth */
    case pace:
      if(stop){
        state = end;
        stop = false;
        break;
      }
      Serial.println("Back and Forth");

      /* stop motors to preserve components */
      digitalWrite(stby, LOW);
      delay(500);

      /* set motors backwards */
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 100);
      digitalWrite(bin1, LOW);
      digitalWrite(bin2, HIGH);
      analogWrite(pwmb, 80);

      /* go backwards for 1/2 second */
      digitalWrite(stby, HIGH);
      delay(400);
      
      /* stop motors to preserve components */
      digitalWrite(stby, LOW);

      /* set motors forward */
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 190);
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 120);
      delay(400);

      /* go forward for 1/2 second */
      digitalWrite(stby, HIGH);
      delay(600);

      rand_state = random(0,3);
      count += 2;

      // repeat this state four times
      pace_count += 1;
      if(pace_count>1){
        if(rand_state==0)
          state = zig;
        else if(rand_state==1)
          state = zag;
        else
          state = forward;
        pace_count = 0;
      }
      else
        state = pace;
      break;

    /* after pacing back and forth commence zig-zag motion */
    case zig:
      if(stop){
        state = end;
        stop = false;
        break;
      }
      Serial.println("zig");

      // stop motor to preserve components
      digitalWrite(stby, LOW);
      delay(1000);

      /* FAILS TO RUN ON AXIS (CORD TOO LONG)*/
      /* digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 225);  

      digitalWrite(bin1, LOW);
      digitalWrite(bin2, HIGH);
      analogWrite(pwmb, 100); */
      
      // zig-zag motion
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 225);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 100);
      digitalWrite(stby, HIGH);
      delay(1200);

      // spin for a random amount of time (0 - 2s)
      rand_move = random(0,2);
      delay(rand_move * 1000);
      count += rand_move;

      // Transition to next state, depending on elapsed time
      if(count>20)
        state = end;
      else
        state = pace;
      break;

    case zag:
      if(stop){
        state = end;
        stop =false;
        break;
      }
      Serial.println("zag");

      // stop motor to preserve components
      digitalWrite(stby, LOW);
      delay(500);

      // zig-zag motion
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 225);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 100);

      digitalWrite(stby, HIGH);
      delay(1200);

      // spin for a random amount of time (0 - 2s)
      rand_move = random(0,2);
      delay(rand_move * 1000);
      count += rand_move;

      // Transition to next state, depending on elapsed time
      if(count>20)
        state = end;
      else
        state = pace;
      break;

    case end:
      // Reset all counters, print that process is over
      Serial.println("End");
      Serial.print("Count: ");
      Serial.println(count);
      count = 0;
      k = 500;
      digitalWrite(stby, LOW);
      

      // After ten seconds, repeat process
      delay(20000);
      state = set;
      break;
  }
}

// Interrupt for wheel button, stops motion
void wheelButtonPress() {
  if(state==pace || state==zig || state==zag || state==forward){
    state = end;
    stop = true;
    Serial.println("Clocky motion stopped");
  }
  else if(wheelMode){
    Serial.println("Wheel icon disabled");
    wheelMode = false;  
  }
  else{
    Serial.println("Wheel icon enabled");
    wheelMode = true;
  }
}

// Interrupt for alarm button
void alarmButtonPress() {
  if(alarm){
    Serial.println("Alarm sound disabled");
    alarm = false;
  }
  if(state == setup){
    if(alarmMode){
      Serial.println("AM icon disabled");
      wheelMode = false;
    }
    else{
      Serial.println("AM icon enabled");
      wheelMode = true;
    }
  }
}
