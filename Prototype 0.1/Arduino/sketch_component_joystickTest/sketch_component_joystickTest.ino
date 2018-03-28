/**
 * Test Joystick
 * 
 * Date Written: July 17, 2017
 *
 * Arduino Board: UNO
 * 
 * Circuit: 
 * *  All Vin to 5V
 * *  X-axis to A0 pin
 * *  Y-axis to A2 pin
 * *  Z-axis to A4 pin
 * *  All Vout to GND
 */

#define pot1 A0
#define pot2 A1
#define pot3 A2

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.print("X = ");
    Serial.print(analogRead(pot1));
    Serial.print(", Y = ");
    Serial.print(analogRead(pot2));
    Serial.print(", Z = ");
    Serial.println(analogRead(pot3));
}
