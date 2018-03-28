/**
 * Stepper Motor Speed Test
 * 
 * Date written: July 17, 2017
 * 
 * Arduino Board: UNO
 * 
 * Circuit: (using SR3-mini Step Motor Drive)
 * *  STEP+ and DIR+ pin to 5V
 * *  STEP- pin to Arduino digital PIN 3
 * *  DIR- pin to Arduino digital PIN 4
 * *  Connect V+ and V- accordingly to 24V power supply
 */

#define stepPin 7
#define dirPin 8

// change customDelay to vary speed
// larger value will move motor slower, smaller value will move motor faster
// ***NOT RECOMMENDED TO GO BELOW 450***
int customDelay = 1000;

void setup() {
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < 10000; i++) {
    digitalWrite(dirPin, LOW);          // move counter-clockwise, change to HIGH for clockwise movement
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(customDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(customDelay);
  }
  for (int i = 0; i < 10000; i++) {
    digitalWrite(dirPin, HIGH);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(customDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(customDelay);
  }
  
}
