/**
 * Off Signal Test 
 * 
 * TESTED with multiple changes after (no sequence to run)
 * 
 * Arduino Board: UNO
 * 
 *  ***Must use regulator 9V to ensure sufficient voltage to run off signal***
 */

// Digital PINS
#define OFF_BUTTON 7
#define OFF_SIGNAL 8

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
    delay(1000);
    return true;
}
