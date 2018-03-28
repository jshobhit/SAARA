/**
 * Off Sequence Test (UNTESTED)
 * 
 * Arduino Board: MEGA
 * 
 *  ***Must use regulator 9V to ensure sufficient voltage to run off signal***
 */

// Digital PINS
#define numOfSteppers 4                  // SM1 SM2 EM1 HM1
const int stepperDirPins[numOfSteppers]  = {22, 26, 30, 34};
const int stepperStepPins[numOfSteppers] = {23, 27, 31, 35};
const int stepperACW[numOfSteppers]      = {24, 28, 32, 36};
const int stepperCW[numOfSteppers]       = {25, 29, 33, 37};
const int homeSteps[numOfSteppers]       = {10, 10, 10, 10};

#define OFF_BUTTON 6
#define OFF_SIGNAL 7

#define SM1 0
#define SM2 1
#define EM1 2
#define HM1 3

const int setDelay = 3000;

void setup() {
    pinMode(OFF_BUTTON, INPUT);
    pinMode(OFF_SIGNAL, OUTPUT);
    digitalWrite(OFF_SIGNAL, LOW);
}

void loop() {
    if (digitalRead(OFF_BUTTON)) {
        if (runToHome()) {
            digitalWrite(OFF_SIGNAL, HIGH); // Arduino power cut off at this point
        }
    }
}

bool runToHome() {
    if(homePos(HM1, 1)) {
        if(homePos(EM1, 1)) {
            if(homePos(SM2, 1)) {
                if(homePos(SM1, 1)) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    // move tip to base
    // hit a/cw limit switch then move set number of steps to home position
    // for each motor (possibly stop at limit switch / hard limit)
}

// dir -> limit switch to hit, ACW == 0, CW == 1
bool homePos(int motor, int dir) {
    if (dir == 0) {
        digitalWrite(stepperDirPins[motor], LOW);
    } else {
        digitalWrite(stepperDirPins[motor], HIGH);
    }
    while (!digitalRead(stepperCW[motor]) && !digitalRead(stepperACW[motor])) {
        digitalWrite(stepperStepPins[motor], HIGH);
        delayMicroseconds(setDelay);
        digitalWrite(stepperStepPins[motor], LOW);
        delayMicroseconds(setDelay);
    }
    if (dir == 0) {
        digitalWrite(stepperDirPins[motor], HIGH);
    } else {
        digitalWrite(stepperDirPins[motor], LOW);
    }
    for(int i = 0; i < homeSteps[motor]; i++) {
        digitalWrite(stepperStepPins[motor], HIGH);
        delayMicroseconds(setDelay);
        digitalWrite(stepperStepPins[motor], LOW);
        delayMicroseconds(setDelay);
    }
}
