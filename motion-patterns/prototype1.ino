// finite states 
unsigned char state;
enum {forward, spin, move, end};

// Button Pins
const int buttonPinA = 2; // External interrupt 0
const int buttonPinB = 3; // External interrupt 1

// Motor A pins
const int pwma = 10;
const int ain1 = 9;
const int ain2 = 12;

// Motor B pins
const int bin2 = 3;
const int pwmb = 6;
const int bin1 = 11;

// stby pins
const int stby = 13;

// random variables
long rand_spin = 0;
long rand_move = 0;
long rand_stop = 0;

// counter variabless
unsigned int count=0;
unsigned int spin_count=0;

// face button modes
bool wheelMode = false;

void setup() {
  Serial.begin(9600);

  // Configure pins as inputs with internal pull-up resistors
  pinMode(buttonPinA, INPUT_PULLUP);
  pinMode(buttonPinB, INPUT_PULLUP);

  // Attach the same ISR to both interrupts
  attachInterrupt(digitalPinToInterrupt(buttonPinA), handleButtonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPinB), handleButtonPress, FALLING);

  // Set output pins to motors
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwmb, OUTPUT);

  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwma, OUTPUT);
}


 /* Finite states that switch through spinning and moving, */
 /* process ends after 45 seconds                          */
void loop() {
  
  // Delay 15 seconds so i can unplug blue wire
  delay(15000);

  switch(state){
    case forward:
      Serial.println("Move forward");
      digitalWrite(stby, LOW);

      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 200);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      
      digitalWrite(stby, HIGH);

      // Move forward for 4 seconds
      delay(4000);
      count+=4;
      state = spin;
      break;

    case spin:

      // Move right motor backward and left motor forward (spin)
      Serial.println("Spin");

      // stop motor to preserve components
      digitalWrite(stby, LOW);
      delay(400);
      
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwma, 200);

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      
      digitalWrite(stby, HIGH);

      // Spin for random amount of time (0.5 - 1s)
      rand_spin = random(5, 10);
      delay(rand_spin * 100);
      count+=(rand_spin/10);
      spin_count+=1;

      // Transition to next state, depending on elapsed time
      if(count>20)
        state = end;
      if(spin_count<=1)
        state = forward;
      else
        state = move;
      break;

    case move:
      Serial.println("Move");

      // stop motor to preserve components
      digitalWrite(stby, LOW);
      delay(400);

      // Move forward
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwma, 200);  

      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmb, 200);
      digitalWrite(stby, HIGH);
  
      // Move forward for a random amount of time (4 - 8s)
      rand_move = random(4,8);
      delay(rand_move * 1000);
      count += rand_move;

      // Stop motors for a random amount of time (0 - 2s)
      analogWrite(pwma, 0);
      analogWrite(pwmb, 0);
      rand_stop = random(0,2);
      delay(rand_stop * 1000);
      count += rand_stop;

      // Transition to next state, depending on elapsed time
      if(count>20)
        state = end;
      else
        state = spin;
      break;

    case end:
      // Reset all counters, print that process is over
      Serial.println("End");
      Serial.print("Count: ");
      Serial.println(count);
      count = 0;
      spin_count = 0;
      digitalWrite(stby, LOW);
      

      // After ten seconds, repeat process
      //delay(10000);
      //state = forward;
      break;
  }
}

// Interrupt for both buttons, stops motion
void handleButtonPress() {
  if(state != end){
    state = end;
    Serial.println("Clocky motion stopped");
  }
  if(wheelMode){
    Serial.println("Wheel icon disabled");
    wheelMode = false;
  }
  else{
    Serial.println("Wheel icon enabled");
    wheelMode = true;
  }
}
