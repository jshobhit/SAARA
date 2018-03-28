/**
 * Inverse Kinematics Method
 * 
 * Arduino board: MEGA
 * 
 * *** NOT READY FOR TESTING ***
 */

// Digital PINS
#define numOfSteppers 4                  // SM1 SM2 EM1 HM1
const int stepperDirPins[numOfSteppers]  = {22, 26, 30, 34};
const int stepperStepPins[numOfSteppers] = {23, 27, 31, 35};
const int stepperACW[numOfSteppers]      = {24, 28, 32, 36};
const int stepperCW[numOfSteppers]       = {25, 29, 33, 37};
const int startSteps[numOfSteppers]      = {10, 10, 10, 10};

// Analog PINS
#define PX 0  // LR joystick
#define PY 1  // UD joystick
#define PZ 2  // twist joystick

// Input Variables
int XVal = 0;
int YVal = 0;
int ZVal = 0;
bool isLimitACW_Pressed[numOfSteppers] = {false, false, false, false};
bool isLimitCW_Pressed[numOfSteppers]  = {false, false, false, false};

// Prgm Variables
int motorID = 0;
long customDelay = 10000;
bool isMoving = false;

// Constants
const long FORWARD_THRES  = 520;
const long BACKWARD_THRES = 480;
const int LSM1 = 0;
const int LSM2 = 307.5;
const int LEM1 = 307.5;
const float k = 0;
const float offSM1 = 90.0;
const float offSM2 = 45.0;
const float offEM1 = 0.0;

// Inverse Kinematics Variables
int steps[numOfSteppers - 1] = {0, 0, 0};
float AM[numOfSteppers - 1] = {0.0, 0.0, 0.0};  // to calibrate before use; actual rotation from limit switch
float qM[numOfSteppers - 1] = {0.0, 0.0, 0.0};  // rotation from X-axis

float dPos[3] = {};
float matJ[3][3] = {};


void setup() {
    for(int n = 0; n < numOfSteppers; n++) {
        pinMode(stepperDirPins[n], OUTPUT);
        pinMode(stepperStepPins[n], OUTPUT);
        pinMode(stepperACW[n], INPUT);
        pinMode(stepperCW[n], INPUT);
    }
    // calibration - motor by motor, or "simultaneously"
    for (int i = 0; i < numOfSteppers; i++) {
        if(!startPos(i)) {
            break; // to send error somewhere
        } 
    }
}

void loop() {
    XVal = analogRead(PX);
    YVal = analogRead(PY);
    ZVal = analogRead(PZ);

    dPos[0] = convert_dPos(XVal);
    dPos[1] = convert_dPos(YVal);
    dPos[2] = convert_dPos(ZVal);

    matJ[0][0] = LEM1*cos(qSM1)*sin(qSM2+qEM1)+LSM2*cos(qSM1)*sin(qSM2);
    matJ[0][1] = LEM1*sin(qSM1)*cos(qSM2+qEM1)+LSM2*sin(qSM1)*cos(qSM2);
    matJ[0][2] = LEM1*sin(qSM1)*cos(qSM2+qEM1);
    matJ[1][0] = LEM1*sin(qSM1)*sin(qSM2+qEM1)+LSM2*sin(qSM1)*sin(qSM2);
    matJ[1][1] = -LEM1*cos(qSM1)*cos(qSM2+qEM1)-LSM2*cos(qSM1)*cos(qSM2);
    matJ[1][2] = -LEM1*cos(qSM1)*cos(qSM2+qEM1);
    matJ[2][0] = 0.0;
    matJ[2][1] = -LEM1*sin(qSM2+qEM1)-LSM2*sin(qSM2);
    matJ[2][2] = -LEM1*sin(qSM2+qEM1);

    
}

float convert_dPos(int val) {
    if (val < BACKWARD_THRES) {
        val = BACKWARD_THRES - val;
        return k * val;
    } else if (val > FORWARD_THRES) {
        val = val - FORWARD_THRES;
        return k * val;
    } else {
        return 0.0;
    }
}

bool startPos(int motor) {
    /*if (dir == 0) {
        digitalWrite(stepperDirPins[motor], LOW);
    } else {*/
        digitalWrite(stepperDirPins[motor], HIGH);
    //}
    while (!digitalRead(stepperCW[motor])) {
        runMotor(motor, setDelay);
    }
    if (digitalRead(stepperCW)) {
        steps[motor] = 0;
        AM[motor] = 0.0;
    }
    /*if (dir == 0) {
        digitalWrite(stepperDirPins[motor], HIGH);
    } else {*/
        digitalWrite(stepperDirPins[motor], LOW);
    //}
    for(int i = 0; i < startSteps[motor]; i++) {
        runMotor(motor, setDelay);
        steps[motor] += 1;
        AM[motor] = steps[motor] * angle;

    }
}

void runMotor(int motor, long delay) {
    digitalWrite(stepperStepPins[motor], HIGH);
    delayMicroseconds(delay);
    digitalWrite(stepperStepPins[motor], LOW);
    delayMicroseconds(delay);
}