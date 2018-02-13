//Declaring the libraries used
#include <Wire.h>
#include <Servo.h>
#include <String.h>

#define HOME_BUTTON 41 //Pin where home button is assigned to 
#define numOfSteppers 4                  // SM1 SM2 EM1 EM2
#define MODEPIN 39
#define CALIPIN 42
#define JOYSTICK_BUTTON 38

/*For the usage of home button*/
String steps = "steps";
String home = "home";
int count; 

/*Accelerometer declaration*/
#define accel_module (0x53) 
byte values[6] ;
char output[512];

//declation of variables and pin numbers

float pos = 60.0;
volatile float hm1Angle = 60; 
float pos1 = 80; 

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
  pinMode(em2.prox, INPUT);

  pinMode(39, INPUT_PULLUP); 
  pinMode(41, INPUT_PULLUP);
  
  hm1.attach(12);
  hm1.write(hm1Angle);
  
  grip.attach(11);
  grip.write(pos1);

  initialize_motors(); 
  
}
int homebut = 0;

void loop(){
/*I2c communication to request reading from accelerometer back to arduino*/ 
 Wire.beginTransmission(accel_module);
 Wire.write(xyzregister);
 Wire.endTransmission();

 Wire.beginTransmission(accel_module);
 Wire.requestFrom(accel_module, 6);

 readVal();

 /*Check if homebutton is pressed. If it is pressed, return to initial position or custom position(Long pressed home button)*/ 
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
  
/* Reading the value of joystick into x and y variables*/
  y = analogRead(A0); 
  x = analogRead(A1);

/*Reading the value of accelerometer and putting it into x_accel and y_accel*/
  y_accel = ((int)values[3] << 8) | values[2];
  x_accel = ((int)values[1] << 8) | values[0];

/*Joystick top button control to check for HIGh and LOW*/   
  buttoncheck();
   if (modeFlag){ 
    delay(100);
    mode1 = digitalRead(MODEPIN);
    if(mode1 == modeFlag){
      mode = !mode;
      modeFlag = 0;
    }
    else{
      modeFlag = 0;
    }
   }

/*Changing of mode(Mode 1 and mode 2)
  - Mode 1(Up down movement, Fwd/Bw movement and sm1 rotation)
  - Mode 2(Gripper open close, hm1 movement and em2 rotation) */   
  if (mode){
    if (digitalRead(JOYSTICK_BUTTON)) /*If joystick button is press, hm1 will be activated else gripper open close*/ 
    { 
      hm1move();   
    }
    else{
      grip1();
    }

    em2move(); 
  }

  else 
  {
    if(digitalRead(JOYSTICK_BUTTON)){/*If joystick button is press, fwd/bwk movement will be activated else updown movement is applied*/
      FwdBk();      
    }
    else{
      maxReach();
    }
  
    /*Rotation of SM1(Left and right)*/ 
    if (x < 450){
      digitalWrite(sm1.dirpin, LOW);
      makeStep(sm1.pin, 600);
      sm1.step--;
      }
  
    else if(x > 600){
      digitalWrite(sm1.dirpin, HIGH); 
      makeStep(sm1.pin, 600);
      sm1.step++;
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

void motorHome(int pin, int steps, int home){
  int correction;
  if(steps > home){
    switch(pin){
      case 47:
        digitalWrite(sm1.dirpin, LOW);
        correction = steps - home;
        Serial.print("SM1 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 49:
        digitalWrite(sm2.dirpin, HIGH);
        correction = steps - home;
        Serial.print("SM2 moving -"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 51: 
        digitalWrite(em1.dirpin, LOW);
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
      case 47:
        digitalWrite(sm1.dirpin, HIGH);
        correction = home - steps;
        Serial.print("SM1 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 49:
        digitalWrite(sm2.dirpin, LOW);
        correction = home - steps;
        Serial.print("SM2 moving +"); Serial.print(correction); Serial.println(" steps.");
        break;
      case 51: 
        digitalWrite(em1.dirpin, HIGH);
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
    makeSteps(pin, 1000, correction);
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

void readVal(){
  int j = 0;
  while(Wire.available()){
    values[j] = Wire.read();
    j++;
  }
}

void UpDown(){
   if (y > 600 && x_accel > 23){ 
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);  
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step -= 5;
    if (em2.step >3276 && em2.step < 4761){
    hm1.write(hm1Angle);
    hm1Angle+= 0.3;
    delayMicroseconds(100);
    }    
  }

  else if(y > 600 && x_accel < 23){
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step += 5;
    if (em2.step >3276 && em2.step < 4761){
    hm1.write(hm1Angle);
    hm1Angle+= 0.4;
    delayMicroseconds(100);
    }    
  }
   
   if(y<450 && x_accel > 23){
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step += 10;
    sm2.step += 5;
    if (em2.step >3276 && em2.step < 4761){
    hm1.write(hm1Angle);
    hm1Angle-= 0.3;
    delayMicroseconds(100);
    }   
  }

  else if(y<450 && x_accel < 23){
    digitalWrite(em1.dirpin, HIGH); 
    digitalWrite(sm2.dirpin, HIGH); 
    makeScaledStep(em1.pin,sm2.pin, 800, 0.5);
    em1.step += 10;
    sm2.step -= 5;
    if (em2.step >3276 && em2.step < 4761){
    hm1.write(hm1Angle);
    hm1Angle-= 0.4;
    delayMicroseconds(100);
    }    
  }
}

void down(){
  if (y > 600 && x_accel > 23){ 
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);  
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step -= 5;
    if (em2.step >3276 && em2.step < 4761){
      hm1.write(hm1Angle);
      hm1Angle+= 0.3;
      delayMicroseconds(100);
    }    
  }

  else if(y > 600 && x_accel < 23){
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, LOW); 
    makeScaledStep(em1.pin, sm2.pin, 800, 0.5);
    em1.step -= 10;
    sm2.step += 5;
    if (em2.step >3276 && em2.step < 4761){
      hm1.write(hm1Angle);
      hm1Angle+= 0.4;
      delayMicroseconds(100);
    }    
  }
}

void maxReach(){
  if (em1.step <= 5000){
    UpDown();
 }
    if (em1.step >= 5000){ 
      down(); 
    } 
}

void FwdBk(){
    if (y > 600 && stepCountsm2 >= 4300 && (stepCountem1 >= 600 || em1.step >= 600) ){ 
      digitalWrite(em1.dirpin, LOW);
      digitalWrite(sm2.dirpin, HIGH);  
      makeScaledStep(sm2.pin, em1.pin, 800, 0.9);
      em1.step -= 9;
      sm2.step -= 10;
      stepCountem1 -= 9;
      stepCountsm2 -= 10;
      
      if (em2.step >3276 && em2.step < 4761){
        hm1.write(hm1Angle);
        hm1Angle-= 0.05;
        delayMicroseconds(100); 
    }      
    }

    else if(y > 600 && stepCountsm2 <= 4300 &&  (stepCountem1 >= 600 || em1.step >= 600)){
      digitalWrite(em1.dirpin, LOW);
      digitalWrite(sm2.dirpin, HIGH); 
      makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
      em1.step -= 5;
      sm2.step -= 10;
      stepCountem1 -= 5;
      stepCountsm2 -= 10;
      if (em2.step >3276 && em2.step < 4761){
        hm1.write(hm1Angle);
        hm1Angle+= 0.05;
        delayMicroseconds(100); 
    }      
    }
   
   if(y< 450 && stepCountsm2 >= 4300 && (stepCountem1 <=5000 || em1.step <= 5000)){
      digitalWrite(em1.dirpin, HIGH);
      digitalWrite(sm2.dirpin, LOW); 
      makeScaledStep(sm2.pin, em1.pin, 800, 0.9);
      em1.step += 9;
      sm2.step += 10;
      stepCountem1 += 9;
      stepCountsm2 += 10;
      if (em2.step >3276 && em2.step < 4761){
        hm1.write(hm1Angle);
        hm1Angle+= 0.05;
        delayMicroseconds(100); 
      }         
    }

    else if(y<450 && stepCountsm2 <= 4300 && (stepCountem1 <= 5000 || em1.step <= 5000)){
      digitalWrite(em1.dirpin, HIGH); 
      digitalWrite(sm2.dirpin, LOW); 
      makeScaledStep(sm2.pin, em1.pin, 800, 0.5);
      em1.step += 5;
      sm2.step += 10;
      stepCountem1 += 5;
      stepCountsm2 += 10;
      if (em2.step >3276 && em2.step < 4761){
        hm1.write(hm1Angle);
        hm1Angle-= 0.05;
        delayMicroseconds(100); 
      }      
    }
}


void buttoncheck(){
  modeFlag = digitalRead(MODEPIN);
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
  makeSteps(em2.pin, 600, 3500); 

  em2.step = 3500; 
  em2.home = 3500;
  
  hm1.write(160);
  
  digitalWrite(em1.dirpin, LOW);
  while (!digitalRead(em1.pin)) {
    makeStep(em1.pin, 600);
  }
  em1.step = 600;
  digitalWrite(em1.dirpin, HIGH);
  makeSteps(em1.pin, 600, 2000);
  em1.step = 2000;
  em1.home = 2000;

  digitalWrite(sm1.dirpin, LOW);
  while (!digitalRead(sm1.pin)) {
    makeStep(sm1.pin, 600);
  }
  sm1.step = 0;
  digitalWrite(sm1.dirpin, HIGH);
  makeSteps(sm1.pin, 600, 2500);
  sm1.step = 2500;
  sm1.home = 2500;
  
  digitalWrite(sm2.dirpin, HIGH);
  while (!digitalRead(sm2.pin)) {
    makeStep(sm2.pin, 600);  
  }
  sm2.step = 0;
  digitalWrite(sm2.dirpin, LOW);
  makeSteps(sm2.pin, 600, 3000);
  sm2.step = 3000;
  sm2.home = 3000;
  
  hm1.write(pos);   
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

void em2move()
{ 
  if (x > 600)
  {
    digitalWrite(em2.dirpin, LOW);
    makeStep(em2.pin, 600); 
    em2.step--;  
  }

  else if (x < 500) 
  {
    digitalWrite(em2.dirpin, HIGH); 
    makeStep(em2.pin, 600); 
    em2.step++; 
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
    grip.write(85); 
  }
}0

void hm1move()
{
  if (y > 600){
    if (hm1Angle > 20 && hm1Angle <160 || hm1Angle >= 160 ){ 
    hm1Angle -= 0.5;
    hm1.write(hm1Angle);
    delay(10);  
  }
}
  else if (y< 450){
    if (hm1Angle > 20 && hm1Angle < 160 || hm1Angle <= 20){ 
    hm1Angle += 0.5;  
    hm1.write(hm1Angle);
    delay(10);
    }
  }
} 
