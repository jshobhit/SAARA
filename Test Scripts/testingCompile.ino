// Calibration at 41 i.e right blue
// Mode Change at 42 i.e left blue
//
#include <Wire.h>
#include <Servo.h>
#include <String.h>
#define HOME_BUTTON 41

String steps = "steps";
String home = "home";
int count; 

#define accel_module (0x53) // accelerometer

byte values[6] ;
char output[512];

float pos = 60.0;
volatile float hm1Angle = 60; 
float pos1 = 80; 
#define numOfSteppers 4                  // SM1 SM2 EM1 EM2
//#define MODEPIN 40
#define CALIPIN 42
#define JOYSTICK_BUTTON 31

#define TEST 1

const int stepperDirPins[numOfSteppers]  = {46, 48, 51, 52};
const int stepperStepPins[numOfSteppers] = {47, 49, 50, 53};
int x, y;

typedef struct {
  int pin;
  int speed = 2000;
  int dir = HIGH;
  int dirpin;
  int prox;
  int step = 0;
  int home;
  int enable; 
} motor;

motor em1, em2, sm1, sm2;
Servo grip, hm1;

int angle = 0;
bool cali = false;
int mode = 0;
int mode1; 

volatile int modeFlag = 0;
volatile int caliFlag = 0;
int val=0;
int xyzregister = 0x32;

int y_accel, x_accel; 
int stepCount;

int stepCountem1 = 2000; //Initialise stepcount for FwdBk (em1 = 2000)
int stepCountsm2 = 3000; //Initialise stepcount for FwdBk (sm2 = 3500)
void setup() {
  Serial.begin(9600);
  Wire.begin();
  //accel.setRange(ADXL345_RANGE_16_G);

  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(16);
  Wire.endTransmission();
  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  
  for (int n = 0; n < numOfSteppers; n++) {
    pinMode(stepperDirPins[n], OUTPUT);
    pinMode(stepperStepPins[n], OUTPUT);

  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  sm1.pin = 12;
  sm2.pin = 10;
  em1.pin = 8;
  em2.pin = 53;
  
  sm1.dirpin = 13;
  sm2.dirpin = 11;
  em1.dirpin = 9;
  em2.dirpin = 52;
  
  sm1.prox = 2;
  sm2.prox = 3;
  em1.prox = 4;
  em2.prox = 5;

  sm1.enable = 51; 
  sm2.enable = 52; 
  em1.enable = 53; 
  
  pinMode(sm1.prox, INPUT);
  pinMode(sm2.prox, INPUT);
  pinMode(em1.prox, INPUT);
  pinMode(em2.prox, INPUT);

  pinMode(sm1.enable, OUTPUT); 
  pinMode(sm2.enable, OUTPUT); 
  pinMode(em1.enable, OUTPUT); 
  
  pinMode(sm1.pin, OUTPUT); 
  pinMode(sm1.dirpin, OUTPUT); 
  pinMode(sm2.pin, OUTPUT); 
  pinMode(sm2.dirpin, OUTPUT); 
  pinMode(em1.pin, OUTPUT); 
  pinMode(em1.dirpin, OUTPUT); 
  pinMode(39, INPUT_PULLUP); 
  pinMode(41, INPUT_PULLUP);
  hm1.attach(12);
  hm1.write(hm1Angle);
  
  grip.attach(11);
  grip.write(pos1);
}
//  initialize_motors(); 
int homebut = 0;

void loop() {
  digitalWrite(sm1.enable, LOW);
  digitalWrite(sm2.enable, LOW); 
  digitalWrite(em1.enable, LOW); 
 
  Wire.beginTransmission(accel_module);
  Wire.write(xyzregister);
  Wire.endTransmission();

  Wire.beginTransmission(accel_module);
  Wire.requestFrom(accel_module, 6);  

  readVal();

  y = analogRead(A1); 
  x = analogRead(A0);
  y_accel = ((int)values[3] << 8) | values[2];
  x_accel = ((int)values[1] << 8) | values[0];


  FwdBk();
  
  if (x < 350){
    digitalWrite(sm1.dirpin, LOW);
    makeStep(sm1.pin, 5);
    sm1.step--;
  }
  
  else if(x > 600){
    digitalWrite(sm1.dirpin, HIGH); 
    makeStep(sm1.pin, 5);
    sm1.step++;
  }
//  Serial.print("sm2.step");Serial.print(sm2.step); 
//  Serial.print("em1.step");Serial.println(em1.step);
}

void readVal(){
  int j = 0;
  while(Wire.available()){
    values[j] = Wire.read();
    j++;
  }
}

void makeStep(int motor, int speed) {
  digitalWrite(motor, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor, LOW);
  delayMicroseconds(speed);
}

void FwdBk(){
    if (y > 600){ 
      digitalWrite(em1.dirpin, LOW);
      digitalWrite(sm2.dirpin, HIGH);  
      makeScaledStep(sm2.pin, em1.pin, 120, 0.9);
      em1.step -= 9;
      sm2.step -= 10;
      stepCountem1 -= 9;
      stepCountsm2 -= 10;
      
//      if (em2.step >3276 && em2.step < 4761){
//        hm1.write(hm1Angle);
//        hm1Angle-= 0.05;
//        delayMicroseconds(100); 
//    }      
    }

//    else if(y > 600 && stepCountsm2 <= 4300 &&  (stepCountem1 >= 600 || em1.step >= 600)){
//      digitalWrite(em1.dirpin, LOW);
//      digitalWrite(sm2.dirpin, HIGH); 
//      makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
//      em1.step -= 5;
//      sm2.step -= 10;
//      stepCountem1 -= 5;
//      stepCountsm2 -= 10;
//      if (em2.step >3276 && em2.step < 4761){
//        hm1.write(hm1Angle);
//        hm1Angle+= 0.05;
//        delayMicroseconds(100); 
//    }      
  //  }
   
   if(y< 450){
      digitalWrite(em1.dirpin, HIGH);
      digitalWrite(sm2.dirpin, LOW); 
      makeScaledStep(sm2.pin, em1.pin, 120, 0.9);
      em1.step += 9;
      sm2.step += 10;
      stepCountem1 += 9;
      stepCountsm2 += 10;
//      if (em2.step >3276 && em2.step < 4761){
//        hm1.write(hm1Angle);
//        hm1Angle+= 0.05;
//        delayMicroseconds(100); 
//      }         
    }

//    else if(y<450 && stepCountsm2 <= 4300 && (stepCountem1 <= 5000 || em1.step <= 5000)){
//      digitalWrite(em1.dirpin, HIGH);
//      digitalWrite(sm2.dirpin, LOW); 
//      makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
//      em1.step += 5;
//      sm2.step += 10;
//      stepCountem1 += 5;
//      stepCountsm2 += 10;
//      if (em2.step >3276 && em2.step < 4761){
//        hm1.write(hm1Angle);
//        hm1Angle-= 0.05;
//        delayMicroseconds(100); 
//      }      
//    }
//    
}
void makeScaledStep(int motor1, int motor2, int speed, float scale){
  int step1 = 10;
  int step2 = 10;
  step2 = step1*scale;
  makeSteps(motor1, speed, step1);
  makeSteps(motor2, speed, step2);
}
void makeSteps(int motor, int speed, int steps) {
  int x = 1;
  while (x <= steps) {
    makeStep(motor, speed);
    x++;
  }
}
