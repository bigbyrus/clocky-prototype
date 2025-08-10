#include <Arduino.h>
#include <TM1637Display.h>
 
unsigned char state;
enum {set, zigzag, forward, spin, end};

// Button Pins
const int buttonPinA = 2;
const int buttonPinB = 3;
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
long rand_spin = 0;
long rand_time = 0;
// counter variabless
unsigned int count=0;
unsigned int zigzag_count=0;
unsigned int spin_count=0;
// face button modes
bool wheelMode = false;
bool alarmMode = false;
bool alarm = false;
// extra
int k = 820;
bool stop = false;
bool forward_flag = false;

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);

  // Physical Button pins
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

  // Set seed for random function
  randomSeed(analogRead(A0));

  // Set default state
  state = set;
}


 /* Finite states that dictate Clocky's motion pattern */
 /* process ends after 30 seconds                      */
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
      if(k == 830){
        Serial.println("Alarm sound on");
        alarm = true;
        forward_flag = true;
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
      /* if forward_flag is true, strictly move both motors forward */
      if(forward_flag){
      Serial.println("Move forward");
      digitalWrite(stby, LOW);

      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 225);  
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
  
      digitalWrite(stby, HIGH);

      /* Move forward for 4 seconds */
      delay(4000);
      count+=4;
      forward_flag = false;

      /* control */
      if(spin_count == 2)
        state = spin;
      else
        state = zigzag;
      break;
      }
      /* when forward flag is false, motors can move backwards or forwards */
      else{
        Serial.println("Move");
        rand_move = random(0,100);
        if(rand>0){
          digitalWrite(stby, LOW);
          digitalWrite(ain1, HIGH);
          digitalWrite(ain2, LOW);
          analogWrite(pwma, 225);  
          digitalWrite(bin1, HIGH);
          digitalWrite(bin2, LOW);
          analogWrite(pwmb, 200);
          digitalWrite(stby, HIGH);
          /* Move for 4 seconds */
          delay(4000);
          count+=4;
        }
        else{
          digitalWrite(stby, LOW);
          digitalWrite(ain1, LOW);
          digitalWrite(ain2, HIGH);
          analogWrite(pwma, 200);  
          digitalWrite(bin1, LOW);
          digitalWrite(bin2, HIGH);
          analogWrite(pwmb, 200);
          digitalWrite(stby, HIGH);
          /* Move for 4 seconds */
          delay(4000);
          count+=4;
        }
        state = zigzag;
        break;
      }
      

    /* after moving forward the Clocky moves in a zigzag motion */
    case zigzag:
      if(stop){
        state = end;
        stop = false;
        break;
      }
      Serial.println("zigzag");

      /* stop motors to preserve components */
      digitalWrite(stby, LOW);
      delay(100);

      /* zig forward */
      digitalWrite(stby, LOW);
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 200);
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      /* 0.25 seconds */
      digitalWrite(stby, HIGH);
      delay(250);
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 200);
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      delay(900);

      /* stop motors to preserve components */
      digitalWrite(stby, LOW);

      /* zag forward */
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 200);
      digitalWrite(bin1, LOW);
      digitalWrite(bin2, HIGH);
      analogWrite(pwmb, 200);
      delay(50);

      /* 0.25 seconds */
      digitalWrite(stby, HIGH);
      delay(250);
            
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 200);
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      delay(900);

      /* control */
      if(zigzag_count == 0){
        zigzag_count += 1;
        break;
      }
      count+=3;
      if(spin_count == 1){
        spin_count+=1;
        forward_flag = true;
        state = forward;
        break;
      }

      /* ranomly choose 0s, 1s, 2s pause */
      rand_state = random(0,2);
      digitalWrite(stby, LOW);
      delay(rand_state * 1000);
      count += rand_state;

      if(count>30)
        state = end;
      else{
        state = spin;
        zigzag_count = 0;
      }
      break;

    /* spin for [0.5-1s] after zig-zag motion */
    case spin:
      if(stop){
        state = end;
        stop = false;
        break;
      }
      Serial.println("spin");

      // stop motor to preserve components
      digitalWrite(stby, LOW);
      delay(50);
      
      // spin motion
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 200);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      digitalWrite(stby, HIGH);

      // spin for a random amount of time (0.5s - 1s)
      rand_move = random(50,101);
      delay(rand_move * 10);
      count += 1;

      // control
      if(spin_count<2){
        forward_flag = true;
        spin_count += 1;
        state = zigzag;
      }
      else if(count>30)
        state = end;
      else
        state = forward;
      break;

    case end:
      // Reset all counters, print that process is over
      Serial.println("End");
      Serial.print("Count: ");
      Serial.println(count);
      count = 0;
      spin_count = 0;
      k = 820;
      digitalWrite(stby, LOW);
      

      // After ten seconds, repeat process
      delay(20000);
      state = set;
      break;
  }
}

// Interrupt for wheel button, stops motion
void wheelButtonPress() {
  if(state==zigzag || state==spin || state==forward){
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
