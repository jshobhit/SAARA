
#include <Servo.h>

#define numOfSteppers 5                  // SM1 SM2 EM1 HM1 HM2

const int stepperACW[numOfSteppers]      = {24, 28, 32, 36, 10};
const int stepperCW[numOfSteppers]       = {25, 29, 33, 37, 11};

void setup() {
	Serial.begin(9600);
	for(int n = 0; n < numOfSteppers; n++) {
        pinMode(stepperACW[n], INPUT);
        pinMode(stepperCW[n], INPUT);
    }  

    pinMode(5, INPUT); // PB1 Switch
    pinMode(4, INPUT); // PB2 Switch
    pinMode(6, INPUT); // PB3 Switch

    Serial.println("Limit Switch Testing. Enter the Limit Switch Position you wish to test.");
    Serial.println(" Eg. SM1 / SM2 / EM1 / HM1 / HM2?");

}

bool haveInput = false;

void loop() {
	String inStepper;
    	if (Serial.available()) {
        	inStepper = String(Serial.readString());
        	haveInput = true;
    		} 
    	else {
        	haveInput = false;
    }

    if(haveInput){
    	
    	Serial.print("Checking Motor: ");
        Serial.println(inStepper);
        	if (inStepper.equals("SM1") || inStepper.equals("0")) {
            	goto sm1;
        } 
        	else if (inStepper.equals("SM2") || inStepper.equals("1")) {
            	goto sm2;
        } 
        	else if (inStepper.equals("EM1") || inStepper.equals("2")) {
            	goto em1;
        } 
        	else if (inStepper.equals("EM2") || inStepper.equals("3")) {
            	goto em1;
        }
        	else if (inStepper.equals("HM1") || inStepper.equals("4")) {
            	goto hm1;
        }  
	        	else {
            	Serial.println("Invalid motor");
            	haveInput = false;
        }

    }

 sm1: 
 	while(x<40){
 		Serial.print("CW: ");
 		Serial.print(digitalRead(stepperCW[0]));

 		Serial.print("ACW: ");
 		Serial.print(digitalRead(stepperACW[0]));
 		x++;
 		delay(200);
 	}
 	goto end;

 sm2: 
 	while(x<40){
 		Serial.print("CW: ");
 		Serial.print(digitalRead(stepperCW[1]));

 		Serial.print("ACW: ");
 		Serial.print(digitalRead(stepperACW[1]));
 		x++;
 		delay(200);
 	}
 	goto end;

 em1: 
 	while(x<40){
 		Serial.print("CW: ");
 		Serial.print(digitalRead(stepperCW[2]));

 		Serial.print("ACW: ");
 		Serial.print(digitalRead(stepperACW[2]));
 		x++;
 		delay(200);
 	}
 	goto end;

 em2: 
 	while(x<40){
 		Serial.print("CW: ");
 		Serial.print(digitalRead(stepperCW[3]));

 		Serial.print("ACW: ");
 		Serial.print(digitalRead(stepperACW[3]));
 		x++;
 		delay(200);
 	}
 	goto end;

 hm1: 
 	while(x<40){
 		Serial.print("CW: ");
 		Serial.print(digitalRead(stepperCW[4]));

 		Serial.print("ACW: ");
 		Serial.print(digitalRead(stepperACW[4]));
 		x++;
 		delay(200);
 	}
 	goto end;

end: 
	haveInput = false;
}

