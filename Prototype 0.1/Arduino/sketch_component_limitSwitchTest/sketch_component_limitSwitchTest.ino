/**
 * Test Limit Switch
 *
 * Date Written: July 25, 2017
 * 
 * Arduino Board: UNO
 * 
 * Circuit: 
 * *  Connect 5V to 10K resistor
 * *  10K resistor to NC pin of limit switch
 * *  Arduino PIN 7 to NC pin of limit switch
 * *  C pin of limit switch to GND
 * *  Circuit uses built-in LED on Arduino Board (labelled "L")
 */

#define LIMIT_SWITCH_PIN 7

void setup() {
    pinMode(LIMIT_SWITCH_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}
