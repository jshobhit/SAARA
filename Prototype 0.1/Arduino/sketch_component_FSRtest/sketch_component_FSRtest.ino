/**
 * Force Sensitive Resistor Test
 * 
 * Date written: July 12, 2017
 * 
 * Arduino Board: UNO
 * 
 * Circuit:
 * *  5V to any FSR pin
 * *  Other FSR pin to Arduino analog PIN A0 (or digital PIN 7 *may not work*)
 * *  Other FSR pin to 10K resistor
 * *  10K resistor to GND
 */

#define fsrpin 7
#define fsrApin 0

int fsrReading;
int fsrAreading;

void setup() {
    Serial.begin(9600);
}

void loop() {
    fsrReading = digitalRead(fsrpin);
    fsrAreading = analogRead(fsrApin);

    Serial.print("Digital Reading = ");
    if (fsrReading == HIGH)
        Serial.println("HIGH");
    else if (fsrReading == LOW)
        Serial.println("LOW");
    else Serial.println ("ERROR");

    Serial.print("Analog Reading =");
    Serial.println(fsrAreading);

    delay(1000);
}
