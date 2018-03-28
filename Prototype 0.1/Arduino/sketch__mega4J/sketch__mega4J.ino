/**
 * INCOMPLETE, UNTESTED
 * 
 * Arduino Board: MEGA
 * 
 * No. of motors: 4
 * Control: By joint (each joint maximum of 2 motors, 2-axis movement)
 * 
 * SM1 will respond to X-axis of joystick
 * SM2, EM1, HM1 will respond to Y-axis of joystick
 * 
 */

// Digital PINS
#define numOfSteppers 4                  // SM1 SM2 EM1 HM1
const int stepperDirPins[numOfSteppers]  = {22, 26, 30, 34};
const int stepperStepPins[numOfSteppers] = {23, 27, 31, 35};
const int stepperACW[numOfSteppers]      = {24, 28, 32, 36};
const int stepperCW[numOfSteppers]       = {25, 29, 33, 37};

#define CHANGE_BUTTON 4
#define ON_BUTTON     5
#define OFF_BUTTON    6

// Analog PINS
#define PX 0  // LR joystick
#define PY 1  // UD joystick
#define PZ 2  // twist joystick

// Constants
const long FORWARD_THRES  = 623;
const long BACKWARD_THRES = 400;
const long MIN_DELAY      = 500;
const long MAX_DELAY      = 16000;

// Input Variables
int XVal = 0;
int YVal = 0;
int ZVal = 0;
bool isChangeBtnPressed = false;
bool isLimitACW_Pressed[numOfSteppers] = {false, false, false, false};
bool isLimitCW_Pressed[numOfSteppers]  = {false, false, false, false};

// Prgm Variables
int motorID = 0;
long customDelay = 10000;
bool isMovingX = false;
bool isMovingY = false;
bool prevIsChangeBtnPressed = false;

void setup() {
    for(int n = 0; n < numOfSteppers; n++) {
        pinMode(stepperDirPins[n], OUTPUT);
        pinMode(stepperStepPins[n], OUTPUT);
        pinMode(stepperACW[n], INPUT);
        pinMode(stepperCW[n], INPUT);
    }
    pinMode(CHANGE_BUTTON, INPUT);
    pinMode(ON_BUTTON, INPUT);
    pinMode(OFF_BUTTON, INPUT);
}

void loop() {
    XVal = analogRead(PX);
    YVal = analogRead(PY);
    ZVal = analogRead(PZ);

    isChangeBtnPressed = digitalRead(CHANGE_BUTTON);
    if (isChangeBtnPressed) {
        if (isChangeBtnPressed != prevIsChangeBtnPressed) {
            setMotorInUse();
        }
    }
    if (motorID == 0) {
        isMovingX = setMotorVar(XVal, motorID);
        isMovingY = setMotorVar(YVal, motorID + 1);
        makeStep(motorID, isMovingX);
        makeStep(motorID + 1, isMovingY);
    } else {
        isMovingY = setMotorVar(YVal, motorID);
        makeStep(motorID, isMovingY);
    }
    prevIsChangeBtnPressed = isChangeBtnPressed;
}

// custom methods

void setMotorInUse() {
    if (motorID < numOfSteppers - 1) {
        motorID++;
        if (motorID == 1) {
            motorID++;
        }
    } else {
        motorID = 0;
    }
}

bool setMotorVar(int potVal, int motor) {
    int currentDir = digitalRead(stepperDirPins[motor]);
    if (potVal > FORWARD_THRES) {
        if (currentDir != HIGH) {
            digitalWrite(stepperDirPins[motor], HIGH);
        }
        customDelay = map(potVal, FORWARD_THRES, 1023, MAX_DELAY, MIN_DELAY);
        if (digitalRead(stepperCW[motor])) {
            return false;
        } else {
            return true;
        }
    } else if (potVal < BACKWARD_THRES) {
        if (currentDir != LOW) {
            digitalWrite(stepperDirPins[motor], LOW);
        }
        customDelay = map(potVal, 0, BACKWARD_THRES, MIN_DELAY, MAX_DELAY);
        if (digitalRead(stepperACW[motor])) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void makeStep(int motor, bool isMoving) {
    if (isMoving == true) {
        digitalWrite(stepperStepPins[motor], HIGH);
        delayMicroseconds(customDelay);
        digitalWrite(stepperStepPins[motor], LOW);
        delayMicroseconds(customDelay);
    }
}
