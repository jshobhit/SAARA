#include <Wire.h>
#include <Servo.h>
#define accel_module (0x53) // accelerometer
byte values[6] ;
char output[512];

int val=0;
int xyzregister = 0x32;

int y_accel, x_accel; 

Servo hm1, grip; 

int sm2dir = 9;    // output of the stepper motors where dir is connected to pin 4 of uStepper and 8 is connected to pin 3
int sm2step = 8; 
int sm1dir = 13; 
int sm1step = 12;
int em1step = 10;
int em1dir = 11; 

int pb1 = 40; 
int pb2 = 41;
int pb3 = 42;
int pb4 = 43;
int mode_button = 39; 
int joystick_button = 31; 

int countsm1 = 0;
int countsm2 = 0; 
int countem1 = 0;

int x; 
int y; 

float hm1pos = 70; 
float gripperpos = 90; 

volatile int modeFlag = 0;
int mode1; 
int mode = 0;

void setup() {
  Wire.begin();
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

  pinMode(pb1, INPUT);
  pinMode(pb2, INPUT); 
  pinMode(pb3, INPUT);  
  pinMode(pb4, INPUT);
  pinMode(mode_button, INPUT); 
  pinMode(joystick_button, INPUT);  
  pinMode(sm1dir, OUTPUT); 
  pinMode(sm1step , OUTPUT);
  pinMode(sm2dir, OUTPUT); 
  pinMode(sm2step, OUTPUT);
  pinMode(em1dir, OUTPUT);
  pinMode(em1step, OUTPUT);
  Serial.begin(115200);

  hm1.attach(7);
  grip.attach(6); 
  hm1.write(hm1pos);
  grip.write(gripperpos);

}

void loop() {
  Wire.beginTransmission(accel_module);
  Wire.write(xyzregister);
  Wire.endTransmission();
  Wire.beginTransmission(accel_module);
  Wire.requestFrom(accel_module, 6);  

  readVal();

  y_accel = ((int)values[3] << 8) | values[2];
  x_accel = ((int)values[1] << 8) | values[0];

  buttoncheck(); 
  
  y = analogRead(A1);
  x = analogRead(A0);
  if (modeFlag){ 
    delay(100);
    mode1 = digitalRead(39);
    if(mode1 == modeFlag){
      mode = !mode;
      modeFlag = 0;
    }
    else{
      modeFlag = 0;
    }
   }
  
  if(mode){
    if(digitalRead(joystick_button)){
      hm1move();   
    }
    else {
      grip1();  
    }
  }

  else{
    if(digitalRead(joystick_button)){
      upDown();
    }
    else {
      fwdBk();
    }  
    if (x > 550){ 
      digitalWrite(sm1dir, HIGH); 
      makeStep(sm1step, 200); 
    }
    if(x < 450){ 
      digitalWrite(sm1dir, LOW); 
      makeStep(sm1step, 200);   
    }
  }
}

void makeStep(int motor, int speed) {
  digitalWrite(motor, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor, LOW);
  delayMicroseconds(speed);
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

void fwdBk(){
    
  if (y>600 && countsm2 <= 8600) {
    digitalWrite(sm2dir, HIGH);
    digitalWrite(em1dir, LOW); 
    makeScaledStep(sm2step, em1step, 500, 0.4); 
    countsm2+= 10;
    countem1+= 9;  
  }
  else if (y > 600 && countsm2 >= 8600){
    digitalWrite(sm2dir, HIGH);
    digitalWrite(em1dir, LOW); 
    makeScaledStep(sm2step, em1step, 500, 0.6); 
    countsm2+= 10;
    countem1+= 9; 
  }
  else if (y < 350 && countsm2 <= 8600) {
    digitalWrite(sm2dir, LOW);
    digitalWrite(em1dir, HIGH);
    makeScaledStep(sm2step, em1step, 500, 0.4);
    countsm2-=10;
    countem1-=9;  
  }
  else if (y < 350 && countsm2 >= 8600){
    digitalWrite(sm2dir, LOW);
    digitalWrite(em1dir, HIGH); 
    makeScaledStep(sm2step, em1step, 500, 0.6); 
    countsm2+= 10;
    countem1+= 9; 
  }
}
void upDown(){
   if (y > 600 && x_accel >= 23){ 
    digitalWrite(em1dir, LOW);
    digitalWrite(sm2dir, HIGH);  
    makeScaledStep(em1step, sm2step, 500 , 0.5);
    countem1 -= 10;
    countsm2 -= 5;
//    if (em2.step >3276 && em2.step < 4761){
//    hm1.write(hm1Angle);
//    hm1Angle+= 0.3;
//    delayMicroseconds(100);
//    }    
  }

  else if(y > 600 && x_accel < 23){
    digitalWrite(em1dir, LOW);
    digitalWrite(sm2dir, LOW); 
    makeScaledStep(em1step, sm2step, 500, 0.5);
    countem1 -= 10;
    countsm2 += 5;
//    if (em2.step >3276 && em2.step < 4761){
//    hm1.write(hm1Angle);
//    hm1Angle+= 0.4;
//    delayMicroseconds(100);
    }    
//  }
   
   if(y<450 && x_accel > 23){
    digitalWrite(em1dir, HIGH);
    digitalWrite(sm2dir, LOW); 
    makeScaledStep(em1step, sm2step, 500, 0.5);
    countem1 += 10;
    countsm2 += 5;
//    if (em2.step >3276 && em2.step < 4761){
//    hm1.write(hm1Angle);
//    hm1Angle-= 0.3;
//    delayMicroseconds(100);
//    }   
  }

  else if(y<450 && x_accel < 23){
    digitalWrite(em1dir, HIGH); 
    digitalWrite(sm2dir, HIGH); 
    makeScaledStep(em1step,sm2step, 500, 0.5);
    countem1 += 10;
    countsm2 -= 5;
//    if (em2.step >3276 && em2.step < 4761){
//    hm1.write(hm1Angle);
//    hm1Angle-= 0.4;
//    delayMicroseconds(100);
    }    
  //}
}

void readVal(){
  int j = 0;
  while(Wire.available()){
    values[j] = Wire.read();
    j++;
  }
}

void grip1()
{ 
    if (y > 600)
  {
    grip.write(180);
  }

  else if (y < 450)
  { 
    grip.write(0);  
  }
  else {
    grip.write(90); 
  }
}

void hm1move()
{
  if (y > 600){
    if (hm1pos > 20 && hm1pos <160 || hm1pos >= 160 ){ 
    hm1pos -= 0.5;
    hm1.write(hm1pos);
    delay(10);  
  }
}
  else if (y< 450){
    if (hm1pos > 20 && hm1pos < 160 || hm1pos <= 20){ 
    hm1pos += 0.5;  
    hm1.write(hm1pos);
    delay(10);
    }
  }
}void buttoncheck(){
  modeFlag = digitalRead(39);
}
