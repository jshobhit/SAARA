/**
 * COMPLETE, UNTESTED
 * Degree Checker
 * Description: Check the range between the two limit switches of a motor
 * 
 * INPUT: Motor Number
 * OUTPUT: Steps and degrees between limit switches
 * 
 * Arduino Board: MEGA
 * 
 * Note: Motor is left at CW limit switch (Home position or moving after check not written)
 */

// Digital PINS
#define numOfSteppers 4                  // SM1 SM2 EM1 HM1
const int stepperDirPins[numOfSteppers]  = {22, 26, 30, 34};
const int stepperStepPins[numOfSteppers] = {23, 27, 31, 35};
const int stepperACW[numOfSteppers]      = {24, 28, 32, 24};
const int stepperCW[numOfSteppers]       = {25, 29, 33, 25};
 
int buff = 0;

// Input Variables
//bool isLimitACW_Pressed[numOfSteppers] = {false, false, false, false};
//bool isLimitCW_Pressed[numOfSteppers]  = {false, false, false, false};

// Prgm Variables
int motorID = 0;
long customDelay = 1000;
bool isMoving = false;
bool haveInput = false;

void setup() {
    Serial.begin(9600);
    for(int n = 0; n < numOfSteppers; n++) {
        pinMode(stepperDirPins[n], OUTPUT);
        pinMode(stepperStepPins[n], OUTPUT);
        pinMode(stepperACW[n], INPUT);
        pinMode(stepperCW[n], INPUT);
    }
}

void loop() {
    /*String inStepper;
    if (Serial.available()) {
        inStepper = String(Serial.readString());
        haveInput = true;
    }

    if (haveInput) {
        Serial.print("Checking Motor ");
        Serial.println(inStepper);
        if (inStepper.equals("SM1") || inStepper.equals("0")) {
            motorID = 0;
        } else if (inStepper.equals("SM2") || inStepper.equals("1")) {
            motorID = 1;
        } else if (inStepper.equals("EM1") || inStepper.equals("2")) {
            motorID = 2;
        } else if (inStepper.equals("HM1") || inStepper.equals("3")) {
            motorID = 3;
        } else {
            Serial.println("Invalid motor");
        }
		*/
	motorID = 3;

        digitalWrite(stepperDirPins[motorID], LOW);
        while(!limswitch(stepperACW[motorID])) {
            Serial.println("Entering ACW Motor Rotation");
            makeStep(motorID);
        }

        long stepCount = 0;
        digitalWrite(stepperDirPins[motorID], HIGH);
        while(!limswitch(stepperCW[motorID])) {
            Serial.println("Entering CW Motor Rotation");
            makeStep(motorID);
            stepCount++;
            Serial.print("Step count: ");
            Serial.println(stepCount);
        }
        Serial.print("Total steps taken by Motor ");
        //Serial.print(inStepper);
        Serial.print(": ");
        Serial.println(stepCount);

        float angle = stepCount * 1.8;
        Serial.print("Total angle traversed: ");
        Serial.print(angle);
        Serial.println(" degrees");
    }

    // move to home position or back to start


void makeStep(int motor) {
    digitalWrite(stepperStepPins[motorID], HIGH);
    Serial.println("Motor High");
    delayMicroseconds(customDelay);
    digitalWrite(stepperStepPins[motorID], LOW);
    Serial.println("Motor Low");
    delayMicroseconds(customDelay);
}

int limswitch(int dir) {
    if(digitalRead(dir)) {
        delay(350);
        int j = digitalRead(dir);
        return j;
    }
    else 
        return 0;
}
