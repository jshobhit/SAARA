/**
 * Arduino Pins Test
 */

#define potPin A0
#define buttonPin A5
#define totalPins 51

int dirPin[totalPins];
int stepPin[totalPins];

bool isMoving = false;
int pinCheck = 0;
bool prevButton = false;

void setup() {
    for (int i = 0; i < totalPins; i++) {
        dirPin[i] = i + 2;
        stepPin[i] = i + 3;
    }
    for (int n; n < totalPins; n++) {
        pinMode(dirPin[n], OUTPUT);
        pinMode(stepPin[n], OUTPUT);
    }
    pinMode(buttonPin, INPUT);
}

void loop() {
    int potVal = analogRead(potPin);
    bool buttonRead = digitalRead(buttonPin);
    if (buttonRead) {
      if (buttonRead != prevButton) {
        pinCheck++;
      }
    }

    if (potVal < 480) {
        digitalWrite(dirPin[pinCheck], LOW);
        isMoving = true;
    } else if (potVal > 520) {
        digitalWrite(dirPin[pinCheck], HIGH);
        isMoving = true;
    } else {
        isMoving = false;
    }

    if (isMoving == true) {
        digitalWrite(stepPin[pinCheck], HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin[pinCheck], LOW);
        delayMicroseconds(1000);
    }
    prevButton = buttonRead;
}
