// Calibration at 41 i.e right blue
// Mode Change at 42 i.e left blue
//
#include <Wire.h>
#include <Servo.h>
#define accel_module (0x53) // accelerometer

byte values[6] ;
char output[512];

float pos = 90.0;

#define numOfSteppers 4                  // SM1 SM2 EM1 EM2
#define MODEPIN 40
#define CALIPIN 42
#define JOYSTICK_BUTTON 44

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
} motor;

motor em1, em2, sm1, sm2;
Servo grip, hm1;

int angle = 0;
bool cali = false;
int mode = 42;

volatile int modeFlag = 0;
volatile int caliFlag = 0;
int val=0;
int xyzregister = 0x32;

int y_accel, x_accel; 
int stepCount;

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

  sm1.pin = 47;
  sm2.pin = 49;
  em1.pin = 51;
  em2.pin = 53;
  
  sm1.dirpin = 46;
  sm2.dirpin = 48;
  em1.dirpin = 50;
  em2.dirpin = 52;
  
  sm1.prox = 2;
  sm2.prox = 3;
  em1.prox = 4;
  em2.prox = 5;
  
  pinMode(sm1.prox, INPUT);
  pinMode(sm2.prox, INPUT);
  pinMode(em1.prox, INPUT);
  pinMode(41, INPUT_PULLUP);
  pinMode(mode, INPUT_PULLUP);
  pinMode(5, INPUT); 
  hm1.attach(13);
  hm1.write(pos);
  grip.attach(12);
  grip.write(180);

  initialize_motors(); 
  }

void loop(){
 
 Wire.beginTransmission(accel_module);
 Wire.write(xyzregister);
 Wire.endTransmission();

 Wire.beginTransmission(accel_module);
 Wire.requestFrom(accel_module, 6);  
 
  readVal();
  y = analogRead(A0); 
  x = analogRead(A1);
  y_accel = ((int)values[3] << 8) | values[2];
  x_accel = ((int)values[1] << 8) | values[0];

  if(digitalRead(mode)){
  
    if(digitalRead(JOYSTICK_BUTTON)){
      FwdBk();
    }
    else{
      UpDown();
    }
  }
  
  else {
    
    if (x > 600){
      digitalWrite(sm1.dirpin, HIGH);
      makeStep(sm1.pin, 800); 
    }
  
    else if(x < 450){
      digitalWrite(sm1.dirpin, LOW); 
      makeStep(sm1.pin, 800); 
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              Custom Function Blocks 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              Do not modify unless sure
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void readVal(){
  int j = 0;
  while(Wire.available()){
    values[j] = Wire.read();
    j++;
  }
}
//
//
void UpDown(){
   if (y > 600 && y_accel > -170){ 
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW);  
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step += 10;
    sm2.step += 5;
    hm1.write(pos);
    pos+= 0.3;
    delayMicroseconds(100);    
  }

  else if(y > 600 && y_accel < -170){
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step += 10;
    sm2.step -= 5;
    hm1.write(pos);
    pos+= 0.3;
    delayMicroseconds(100);    
  }
  
  else if(y<150 && y_accel > -170){
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step -= 5;
    hm1.write(pos);
    pos-= 0.3;
    delayMicroseconds(100);   
  }

  else if(y<150 && y_accel < -170){
    digitalWrite(em1.dirpin, LOW); 
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin,sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step += 10;
    hm1.write(pos);
    pos-= 0.3;
    delayMicroseconds(100);    
  }
}


void FwdBk(){
    if (y > 600 && sm2.step >= 1620){ 
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);  
    makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
    em1.step -= 5;
    sm2.step += 10;   
  }

  else if(y > 600 && sm2.step <= 1620){
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(sm2.pin, em1.pin, 800, 1.1);
    em1.step -= 5;
    sm2.step += 10;   
  }
  
  else if(y<150 && sm2.step >= 1620){
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
    em1.step += 5;
    sm2.step -= 10;   
  }

  else if(y<150 && sm2.step <= 1620){
    digitalWrite(em1.dirpin, HIGH); 
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(sm2.pin, em1.pin, 800, 1.1);
    em1.step += 5;
    sm2.step -= 10;
  }
}


void buttoncheck(){
//  modeFlag = !digitalRead(41);
//  caliFlag = !digitalRead(42);
}

int initialize_motors(){

  while(!digitalRead(em2.prox)){
  digitalWrite(em2.dirpin, HIGH); 
  makeStep(em2.pin, 600); 
  }
  
  em2.step= 0; 

  digitalWrite(em2.dirpin, LOW); 
  makeStep(em2.pin, 600);
  makeSteps(em2.pin, 600, 4000); 

  em2.step = 4000; 

  hm1.write(0);
  
  digitalWrite(em1.dirpin, LOW);
  while (!digitalRead(4)) {
    makeStep(em1.pin, 600);
  }
  em1.step = 0;
  digitalWrite(em1.dirpin, HIGH);
  makeSteps(em1.pin, 600, 2920);
  em1.step = 2920;
//
//  digitalWrite(sm1.dirpin, LOW);
//  while (!digitalRead(2)) {
//    makeStep(sm1.pin, 600);
//  }
//  sm1.step = 0;
//  digitalWrite(sm1.dirpin, HIGH);
//  makeSteps(sm1.pin, 600, 2500);
//  sm1.step = 2500;

  digitalWrite(sm2.dirpin, HIGH);
  while (!digitalRead(3)) {
    makeStep(sm2.pin, 600);  
  }
  sm2.step = 0;
  digitalWrite(sm2.dirpin, LOW);
  makeSteps(sm2.pin, 600, 2800);
  sm2.step = 2800;

  hm1.write(pos);   


  digitalWrite(em2.dirpin, LOW); 
  digitalWrite(em2.pin, 600); 
  makeSteps(em2.pin, 600, 2000);
  em2.step = 6000;
  }

void makeStep(int motor, int speed) {
  digitalWrite(motor, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor, LOW);
  delayMicroseconds(speed);
}

void makeStep2(int motor1, int motor2, int speed){
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, HIGH);
  delayMicroseconds(800);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  delayMicroseconds(800);
}

void makeDeg(int motor, int speed){
  int divs; 
    switch(motor){
        case 23:
            divs = 50;
            break;
        case 25:
            divs = 100;
            break;
        case 27:
            divs = 50;
            break;
        case 29:
            divs = 51;
            break;
        default:
            Serial.println("Invalid Motor selected for getDegree");
            break;  
    }
    float x;
    x = round(divs / 1.8);
    makeSteps(motor,speed,x);
}

void makeDegs(int motor, int speed, float degs){
  float x = 0;
  while(x <= round(degs)){
    makeDeg(motor, speed);
    x++;
  }
}

void makeDegs2(int motor1, int motor2, int speed, int scale){
  float deg1, deg2;
  deg1 = 1; deg2 = 1;
  deg2 = scale*deg1;
  makeDegs(motor1, speed, deg1);
  makeDegs(motor2, speed, round(deg2));
}

void makeSteps(int motor, int speed, int steps) {
  int x = 1;
  while (x <= steps) {
    makeStep(motor, speed);
    x++;
  }
}

void makeScaledStep(int motor1, int motor2, int speed, float scale){
  int step1 = 10;
  int step2 = 10;
  step2 = step1*scale;
  makeSteps(motor1, speed, step1);
  makeSteps(motor2, speed, step2);
}


/*Format of the function is InvK(parameter that gets increased, Parameter that should stay constant, increase = 1, decrease =0)
void InvK(int inc, int adj, int dir){
  float X, Y;
  em1deg = getDegree(em1.pin, em1.step);
  sm2deg = getDegree(sm2.pin, sm2.step);
  Y = l1(sin(sm2deg*0.0174533)) +  l2(sin((sm2deg*0.0174533)+(em1deg*0.0174533)));
  X = l1(cos(sm2deg*0.0174533)) +  l2(cos((sm2deg*0.0174533)+(em1deg*0.0174533)));

  if(X - inc){
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, LOW);
    makeStep2(sm2.pin, em1.pin, 1000);
  }

}
*/

int getDegree(int motor, int steps) {
  float angle;
  switch(motor){
    case 23:
        angle = (steps*1.8)/50;
        break;
    case 25:
        angle = (steps*1.8)/100;
        break;
    case 27:
        angle = (steps*1.8)/50;
        break;
    case 29:
        angle = (steps*1.8)/51;
        break;
    default:
        Serial.println("Invalid Motor selected for getDegree");
        break;
  }
  return angle;
}

void moveDegrees(int motor, int degree, int speed) {
  int x = 0;
  stepCount = 0;
  while (x <= degree) {
    makeStep(motor, speed);
    stepCount++;
    x = getDegree(motor, stepCount);
  }

}

int DegToSteps(int motor, int deg){
  int steps;
  switch(motor){
      case 23:
          steps = (deg*50)/1.8;
          break;
      case 25:
          steps = (deg*100)/43;
          break;
      case 27:
          steps = (deg*50)/1.8;
          break;
      case 29:
          steps = (deg*51)/1.8;
          break;
      default:
          Serial.println("Invalid Motor selected for getDegree");
          break;
  }
  return round(steps);
}

//void servos(){
//  x = joyX; 
//  y = joyY; 
//  x = map(analogRead(joyX), 0, 1023, 0, 180);
//  y = map(analogRead(joyY), 0, 1023, 0, 180); 
//  hm1.write(x); 
//  grip.write(y);  
//}

