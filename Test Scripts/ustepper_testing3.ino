#include <Wire.h>
#include <Servo.h>
#define accel_module (0x53) // accelerometer
#define HOME_BUTTON 41

byte values[6] ;
char output[512];

int val=0;
int xyzregister = 0x32;

int y_accel, x_accel; 

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
Servo hm1, grip; 
 
int pb1 = 40; 
int pb3 = 42;
int pb4 = 43;
int mode_button = 39; 
int joystick_button = 31; 

int x, y; 

volatile float hm1pos = 70; 
float gripperpos = 90; 

volatile int modeFlag = 0;
int mode1; 
int mode = 0;

int homebut = 0;
String steps = "steps";
String home = "home";
int count; 

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


  sm2.dirpin = 9;    // output of the stepper motors where dir is connected to pin 4 of uStepper and 8 is connected to pin 3
  sm2.pin = 8; 
  sm1.dirpin = 12; 
  sm1.pin = 13;
  em1.pin = 10;
  em1.dirpin = 11;
  em2.pin = 3; 
  em2.dirpin = 2;
  
  sm1.step = 0;
  sm2.step = 0; 
  em1.step = 0;

  sm1.home = 0;
  sm2.home = 0;
  em1.home = 0;
  
  pinMode(pb1, INPUT);
  pinMode(HOME_BUTTON, INPUT); 
  pinMode(pb3, INPUT);  
  pinMode(pb4, INPUT);
  pinMode(mode_button, INPUT); 
  pinMode(joystick_button, INPUT);  
  pinMode(sm1.dirpin, OUTPUT); 
  pinMode(sm1.pin , OUTPUT);
  pinMode(sm2.dirpin, OUTPUT); 
  pinMode(sm2.pin, OUTPUT);
  pinMode(em1.dirpin, OUTPUT);
  pinMode(em1.pin, OUTPUT);
  pinMode(em2.pin, OUTPUT); 
  pinMode(em2.dirpin, OUTPUT);
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

  homebut = ButtonRead(HOME_BUTTON);
  switch(homebut){
    case 1:
      Serial.println("Going to Home Position");  
      gotoHome();
      break;
      break;
    case 2:
      Serial.println("Setting new Home");
      setHomePos(sm1.step, sm2.step, em1.step, em2.step);
      break;
    default:
      break;
  }
  
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
    em2move();
    if(digitalRead(joystick_button)){
      hm1move();   
    }
    else {
      grip1();  
    }
  }

  else{
    if (x > 550){ 
      digitalWrite(sm1.dirpin, HIGH); 
      makeStep(sm1.pin, 600);
      sm1.step++;
      if (y > 600){ 
        makeSteps(sm1.pin, 900, 5);
        sm1.step+=5;
      }
      else if (y < 450){ 
        makeSteps(sm1.pin, 900 , 5);
        sm1.step+=5;
      }

    } 
    else if(x < 450){ 
      digitalWrite(sm1.dirpin, LOW); 
      makeStep(sm1.pin, 600);
      sm1.step--;
      if (y < 450){ 
        makeSteps(sm1.pin, 900, 5);
        sm1.step-=5;
      }
      if (y > 600){ 
        makeSteps(sm1.pin, 900, 5);
        sm1.step-=5;
      }

    }
    if(digitalRead(joystick_button)){
      maxReach();
    }
    else {
      fwdBk();
    }     
  } 
  Serial.println(em2.step);
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
  int step1 = 5;
  int step2 = 5;
  step2 = step1*scale;
  makeSteps(motor1, speed, step1);
  makeSteps(motor2, speed, step2);
}

void fwdBk(){
    
  if (y < 450 && sm2.step <= 1776 && em1.step <= 7000) {
    digitalWrite(sm2.dirpin, HIGH);
    digitalWrite(em1.dirpin, LOW); 
    makeScaledStep(sm2.pin, em1.pin, 400, 0.4); 
    sm2.step+= 5;
    em1.step+= 2;  
    if (em2.step >-1072 && em2.step < 1072){
        hm1.write(hm1pos);
        hm1pos-= 0.01;
        delayMicroseconds(100); 
    }    
  }
  else if (y < 450 && sm2.step >= 1776 && em1.step <= 7000){
    digitalWrite(sm2.dirpin, HIGH);
    digitalWrite(em1.dirpin, LOW); 
    makeScaledStep(sm2.pin, em1.pin, 400, 0.6); 
    sm2.step+= 5;
    em1.step+= 3;
    if (em2.step >-1072 && em2.step < 1072){
        hm1.write(hm1pos);
        hm1pos-= 0.01;
        delayMicroseconds(100); 
    }     
  }
  else if (y > 600 && sm2.step <= 1776 && em1.step >= -2178) {
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, HIGH);
    makeScaledStep(sm2.pin, em1.pin, 400, 0.4);
    sm2.step -= 5;
    em1.step -= 2;
    if (em2.step >-1072 && em2.step < 1072){
        hm1.write(hm1pos);
        hm1pos+= 0.01;
        delayMicroseconds(100); 
    }      
  }
  else if (y > 600 && sm2.step >= 1776 && em1.step >= -2178 ){
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, HIGH); 
    makeScaledStep(sm2.pin, em1.pin, 400, 0.6); 
    sm2.step -= 5;
    em1.step -= 3;
    if (em2.step >-1072 && em2.step < 1072){
        hm1.write(hm1pos);
        hm1pos+= 0.01;
        delayMicroseconds(100); 
    }     
  }
}
void upDown(){
   if (y < 450 && x_accel > 23){ 
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);  
    makeScaledStep(em1.pin, sm2.pin, 400 , 0.6);
    em1.step += 5;
    sm2.step += 3;
    if (em2.step > -1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos+= 0.07;
    delayMicroseconds(100);
    }    
  }

  else if(y < 450 && x_accel < 23){
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 400, 0.6);
    em1.step += 5;
    sm2.step -= 3;
    if (em2.step >-1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos+= 0.07;
    delayMicroseconds(100);
    }    
  }
   
   if(y > 600 && x_accel > 23){
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 400, 0.6);
    em1.step -= 5;
    sm2.step -= 3;
    if (em2.step > -1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos-= 0.07;
    delayMicroseconds(100);
    }   
  }

  else if(y > 600 && x_accel < 23){
    digitalWrite(em1.dirpin, HIGH); 
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(em1.pin,sm2.pin, 400, 0.6);
    em1.step -= 5;
    sm2.step += 3;
    if (em2.step > -1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos-= 0.07;
    delayMicroseconds(100);
    }    
  }
}
void down(){
  if(y > 600 && x_accel > 23){
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 400, 0.6);
    em1.step -= 5;
    sm2.step -= 3;
    if (em2.step >-1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos-= 0.07;
    delayMicroseconds(100);
    }   
  }

  else if(y > 600 && x_accel < 23){
    digitalWrite(em1.dirpin, HIGH); 
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(em1.pin,sm2.pin, 400, 0.6);
    em1.step -= 5;
    sm2.step += 3;
    if (em2.step > -1072 && em2.step < 1072){
    hm1.write(hm1pos);
    hm1pos-= 0.07;
    delayMicroseconds(100);
  }
}
}

void maxReach(){
  if (em1.step <= 6946){
    upDown();
  }
  if (em1.step >= 10000){ 
    down(); 
  } 
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
  if (y < 450){
    if (hm1pos > 20 && hm1pos <160 || hm1pos >= 160 ){ 
    hm1pos -= 0.5;
    hm1.write(hm1pos);
    delay(10);  
  }
}
  else if (y > 600){
    if (hm1pos > 20 && hm1pos < 160 || hm1pos <= 20){ 
    hm1pos += 0.5;  
    hm1.write(hm1pos);
    delay(10);
    }
  }
}

void em2move()
{ 
  if (x < 450)
  {
    digitalWrite(em2.dirpin, LOW);
    makeStep(em2.pin, 600); 
    em2.step--;  
  }
  else if (x > 600) 
  {
    digitalWrite(em2.dirpin, HIGH); 
    makeStep(em2.pin, 600); 
    em2.step++; 
  }
}

void buttoncheck(){
  modeFlag = digitalRead(39);
}

void motorHome(int pin, int steps, int home){
  int correction;
  if(steps > home){
    switch(pin){
      case 13:
        digitalWrite(sm1.dirpin, LOW);
        correction = steps - home;
        Serial.print("SM1 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 8:
        digitalWrite(sm2.dirpin, LOW);
        correction = steps - home;
        Serial.print("SM2 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 10: 
        digitalWrite(em1.dirpin, HIGH);
        correction = steps - home;
        Serial.print("EM1 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 53: 
        digitalWrite(em2.dirpin, HIGH);
        correction = steps - home;
        Serial.print("EM2 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      default:
        Serial.println("Invalid Pin Selected.");
        break;
    }
  }
  else if(steps < home){
    switch(pin){
      case 13:
        digitalWrite(sm1.dirpin, HIGH);
        correction = home - steps;
        Serial.print("SM1 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 8:
        digitalWrite(sm2.dirpin, HIGH);
        correction = home - steps;
        Serial.print("SM2 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 10: 
        digitalWrite(em1.dirpin, LOW);
        correction = home - steps;
        Serial.print("EM1 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 53: 
        digitalWrite(em2.dirpin, LOW);
        correction = home - steps;
        Serial.print("EM2 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;  
      default :
        Serial.println("Invalid Pin Argument");
        break;
    }
  }
  else{}
    makeSteps(pin, 400, correction);
 }


void gotoHome(){
  motorHome(sm1.pin, sm1.step, sm1.home);
  sm1.step = sm1.home;
  motorHome(em1.pin, em1.step, em1.home);
  em1.step = em1.home;
  motorHome(sm2.pin, sm2.step, sm2.home);
  sm2.step = sm2.home;
//  motorHome(em1.pin, em1.step, em1.home);
//  em1.step = em1.home;
  //motorHome(em2.pin, em2.step, em2.home);
  //em2.step = em2.home;
}

void mPrint(String xyz){
  if(xyz == "steps"){
    Serial.println("\n SM1 SM2 EM1 EM2 ");
    Serial.print("  "); Serial.print(sm1.step); Serial.print(" ");
    Serial.print("  "); Serial.print(sm2.step); Serial.print(" ");
    Serial.print("  "); Serial.print(em1.step); Serial.print(" ");
    Serial.print("  "); Serial.print(em2.step); Serial.println(" ");
  }
  else if(xyz == "home"){
    Serial.println("\n SM1 SM2 EM1 EM2 ");
    Serial.print("  "); Serial.print(sm1.home); Serial.print(" ");
    Serial.print("  "); Serial.print(sm2.home); Serial.print(" ");
    Serial.print("  "); Serial.print(em1.home); Serial.print(" ");
    Serial.print("  "); Serial.print(em2.home); Serial.println(" ");
  }
  else{
    //Serial.println("\nInvalid Argument to mPrint");
  }
}

void setHomePos(int x, int y, int z, int a){
  sm1.home = x;
  sm2.home = y;
  em1.home = z;
  em2.home = a;
}

int ButtonRead(int button){
  count = 0;
  while(digitalRead(button)){
    delay(10);
    count++;
  }
  if(count > 100){
    return 2;
  }
  else if(count > 0){
    return 1;
  }
  else{
    return 0;
  }
}

