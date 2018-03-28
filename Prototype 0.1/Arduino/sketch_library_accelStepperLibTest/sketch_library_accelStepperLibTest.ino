/**
 * COMPLETE, TESTED
 * single stepper, accelstepper test
 *
 * Date written: July 18, 2017
 * 
 * Arduino Board: UNO
 *
 * Final decision: to use hardcode due to higher torque but viable option
 * 
 */

#include <AccelStepper.h>

// Digital PINS
#define DIR 7
#define STEP 8

AccelStepper mystepper(AccelStepper::DRIVER, STEP, DIR);

void setup() {
	mystepper.setMaxSpeed(900.0); // default value 1.0
	mystepper.setAcceleration(100.0); // default value 1.0
	mystepper.moveTo(1000000);
}

void loop() {
	if (mystepper.distanceToGo() == 0) {
		mystepper.moveTo(-mystepper.currentPosition());
	}
	mystepper.run();
}
