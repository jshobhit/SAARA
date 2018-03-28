/**
 * Servo Motor Test
 *
 * Date Written: July 3, 2017
 *
 * Arduino Board: UNO
 * 
 * Circuit: 
 * *  Servo pin to Arduino digital PIN 8
 * *  Other two pins to power supply
 */

#include <Servo.h>

Servo servo1;

#define SERVO_PIN 8

int pos = 0;

void setup() {
    Serial.begin(9600);
    servo1.attach(SERVO_PIN);
}

void loop() {
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees in steps of 1 degree
        servo1.write(pos);                // tell servo to go to position in variable 'pos'
        delay(15);                        // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        servo1.write(pos);                // tell servo to go to position in variable 'pos'
        delay(15);                        // waits 15ms for the servo to reach the position
    }
}
