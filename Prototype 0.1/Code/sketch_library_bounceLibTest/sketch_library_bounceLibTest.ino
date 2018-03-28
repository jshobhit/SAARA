/**
 * Bounce library test (UNTESTED)
 * single button reading
 * 
 * Date written: July 20, 2017
 * 
 * Arduino Board: UNO
 * 
 */

#include <Bounce2.h>

#define btnPin 7
#define ledPin 8

Bounce button1 = Bounce();

void setup() {
  // put your setup code here, to run once:
	pinMode(btnPin, INPUT);
	pinMode(ledPin, OUTPUT);

	button1.attach(btnPin);
	button1.interval(5);
}

void loop() {
  // put your main code here, to run repeatedly:
	button1.update();
	if(button1.rose()) {
		digitalWrite(ledPin, HIGH);
		delay(1000);
		digitalWrite(ledPin, LOW);
	}
}
