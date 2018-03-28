/**
 * Inverse Kinematics Method - for first prototype
 * 
 * To do: 
 * - calibrate motors
 * - move motors according to dqM
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
bool invK = false;

// Constants
const long FORWARD_THRES  = 550;
const long BACKWARD_THRES = 470;
const float k = 0;
const float offset[numOfSteppers - 1] = {135.0, 180.0, 0.0};  //in degrees
const float length[numOfSteppers - 1] = {0.0, 0.0, 0.0};  
const float degPerStep = 0.036;         // harmonic drive 1/50

// Inverse Kinematics Variables
float dx = 0.0;
float dy = 0.0;
int currSteps[numOfSteppers - 1] = {0, 0, 0};
float AM[numOfSteppers - 1] = {0.0, 0.0, 0.0};  // to calibrate before use; actual rotation from limit switch
float qM[numOfSteppers - 1] = {0.0, 0.0, 0.0};  // rotation from X-axis
float dqM[numOfSteppers - 1] = {0.0, 0.0, 0.0}; // change in degrees to taken

void setup() {
    for(int n = 0; n < numOfSteppers; n++) {
        pinMode(stepperDirPins[n], OUTPUT);
        pinMode(stepperStepPins[n], OUTPUT);
        pinMode(stepperACW[n], INPUT);
        pinMode(stepperCW[n], INPUT);
    }
    pinMode(CHANGE_BUTTON, INPUT);

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
    //ZVal = analogRead(PZ);

    isChangeBtnPressed = digitalRead(CHANGE_BUTTON);
    if (isChangeBtnPressed) {
        if (isChangeBtnPressed != prevIsChangeBtnPressed) {
            invK = !invK;
        }
    }

    if (invK) {
        // calculate dx, dy
        dx = calculateDpos(XVal);
        dy = calculateDpos(YVal);

        calculateDQM();

    } else {
        moveLR();
    }

}

bool startPos(int motor) {
    digitalWrite(stepperDirPins[motor], HIGH);
    while (!digitalRead(stepperCW[motor])) {
        runMotor(motor, setDelay);
    }
    if (digitalRead(stepperCW)) {
        currSteps[motor] = 0;
        AM[motor] = 0.0;
    }
    
    digitalWrite(stepperDirPins[motor], LOW);
    for(int i = 0; i < startSteps[motor]; i++) {
        runMotor(motor, setDelay);
        currSteps[motor] += 1;
    }
    AM[motor] = steps[motor] * angle;
}

float calculateDpos(int val) {
    if (val > FORWARD_THRES) {
        return map(val, FORWARD_THRES, 1023, 0.0, 1.0);
    } else if (val < BACKWARD_THRES) {
        return map(val, 0, BACKWARD_THRES, -1.0, 0.0);
    } else {
        return 0.0;
    }
}

void calculateDQM() {
    float constraint = qM[0] + qM[1];
    if (constraint == 90.0 || constraint == -90.0 || constraint == 270.0 || constraint == -270.0) {
        constraint += 0.01;
    }

    dqM[0] = (dx + (dy * tan(constraint))) / (length[0] * ((cos(qM[0]) * tan(constraint)) - sin(qM[0]));
    dqM[1] = ((dy - (dqM[0] * length[0] * cos(qM[0]))) / (length[1] * cos(constraint))) - dqM[0];
    dqM[2] = -(dqM[0] + dqM[1]);
}

void moveLR() {
    int currentDir = digitalRead(stepperDirPins[0]);
    if (potVal < BACKWARD_THRES) {
        digitalWrite(stepperDirPins[0], LOW);
        customDelay = 1000;
        if (digitalRead(stepperACW[0])) {
            ;
        } else {
            runMotor(0, customDelay);
        }
    } else if (potVal > FORWARD_THRES) {
        digitalWrite(stepperDirPins[0], HIGH);
        customDelay = 1000;
        if (digitalRead(stepperCW[0])) {
            ;
        } else {
            runMotor(0, customDelay);
        }
    } else {
        ;
    }
}

void runMotor(int motor, long delay) {
    digitalWrite(stepperStepPins[motor], HIGH);
    delayMicroseconds(delay);
    digitalWrite(stepperStepPins[motor], LOW);
    delayMicroseconds(delay);
    if (digitalRead(stepperDirPins[motor]) == HIGH) {
        currSteps[motor] += 1;
    } else {
        currSteps[motor] -= 1;
    }
}