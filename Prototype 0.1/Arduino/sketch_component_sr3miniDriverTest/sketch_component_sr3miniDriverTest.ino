/**
 * SR3-mini Step Motor Drive Test
 * 
 * Date written: July 13, 2017
 * 
 * Arduino Board: UNO
 * 
 * Circuit:
 * *  STEP+ and DIR+ of driver to 5V
 * *  Arduino analog PIN A0 to potentiometer
 * *  Arduino digital PIN 3 to STEP- of driver
 * *  Arduino digital PIN 4 to DIR- of driver
 * *  V+ and V- of driver to 24V power supply
 */

#define potpin A0
#define stepPin 3
#define dirPin 4

const long FORWARD_THRES = 623;
const long BACKWARD_THRES = 400;
const long MIN_DELAY = 500;
const long MAX_DELAY = 16000;

long potVal = 0;
bool isMoving = false;
long customDelay;

void setup() {
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
}

void loop() {
    potVal = analogRead(potpin);

    if (potVal > FORWARD_THRES) {
    digitalWrite(dirPin, HIGH);
    customDelay = map(potVal, FORWARD_THRES, 1023, MAX_DELAY, MIN_DELAY);
    isMoving = true;
    }
    else if (potVal < BACKWARD_THRES) {
    digitalWrite(dirPin, LOW);
    customDelay = map(potVal, 0, BACKWARD_THRES, MIN_DELAY, MAX_DELAY);
    isMoving = true;
    } else {
    isMoving = false;
    }

    if (isMoving == true) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    }
}
