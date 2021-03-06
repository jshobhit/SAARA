#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>

#define SM1Address2 0
#define SM2Address2 2
#define EM1Address2 4
#define EM2Address2 14

#define upDownCountAddress 20
#define upDownCountAddress2 22

#define SM1Address 6
#define SM2Address 8
#define EM1Address 10
#define EM2Address 12

#define HOME_BUTTON 41
#define DRINK_BUTTON 40

byte values[6];
char output[512];

int correction2, correction3;

int relayPin = 53;

int dir;
float upDownCount;
int upDownCountHome, upDownCountHome2;
int em2step = 1171;
int em2home = 1171;

int powerCorrection, powerCorrection2, powerCorrection3, powerCorrection4, totalPower;
int testCorrection, testCorrection2, testCorrection3, testCorrection4, totalCorrection;
int drinkCorrection, drinkCorrection2, drinkCorrection3, drinkCorrection4, drinkTotalCorrection;

int val = 0;
int xyzregister = 0x32;
float y_accel, x_accel, z_accel;
int hm1home;
typedef struct
{
  int pin;
  int speed = 2000;
  int dir = HIGH;
  int dirpin;
  int prox;
  int step = 0;
  int home;
  int enable;
  int home2;
  int powerHome;
} motor;

motor em1, em2, sm1, sm2;
Servo hm1, grip;
int v, r, t, w;
int pb1 = 40;
int pb3 = 42;
int pb4 = 43;
int mode_button = 39;
int joystick_button = 31;

int redLed = 7;
int greenLed = 6;
int blueLed = 5;

int x, y;
float hm1pos = 70;
float gripperpos = 90;

volatile int modeFlag = 0;
volatile int powerFlag = 0;
int mode1;
int mode = 0;
int power1;
int power = 0;
int speed, speed2; 
int timer = 0;
int drinkbut = 0;
int homebut = 0;
String steps = "steps";
String home = "home";
int count;
int sm2Count = 0;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);

  sm2.dirpin = 23; // output of the stepper motors where dir is connected to pin 4 of uStepper and 8 is connected to pin 3
  sm2.pin = 22;
  sm1.dirpin = 12;
  sm1.pin = 13;
  em1.pin = 10;
  em1.dirpin = 11;
  em2.pin = 8;
  em2.dirpin = 9;

  sm1.step = 0;
  sm2.step = 0;
  em1.step = 0;
  em2.step = 0;

  sm1.home = (EEPROM.read(6) * 255) + EEPROM.read(7);
  sm2.home = (EEPROM.read(8) * 255) + EEPROM.read(9);
  em1.home = (EEPROM.read(10) * 255) + EEPROM.read(11);
  em2.home = (EEPROM.read(12) * 255) + EEPROM.read(13);

  sm1.home2 = (EEPROM.read(0) * 255) + EEPROM.read(1);
  sm2.home2 = (EEPROM.read(2) * 255) + EEPROM.read(3);
  em1.home2 = (EEPROM.read(4) * 255) + EEPROM.read(5);
  em2.home2 = (EEPROM.read(14) * 255) + EEPROM.read(15);

  sm1.powerHome = 0;
  sm2.powerHome = -2610;
  em1.powerHome = -2700;
  em2.powerHome = 0;

  upDownCountHome = (EEPROM.read(20) * 255) + EEPROM.read(21);
  upDownCountHome2 = (EEPROM.read(22) * 255) + EEPROM.read(23);

  pinMode(pb1, INPUT);
  pinMode(HOME_BUTTON, INPUT);
  pinMode(pb3, INPUT);
  pinMode(pb4, INPUT);
  pinMode(mode_button, INPUT);
  pinMode(joystick_button, INPUT);
  pinMode(sm1.dirpin, OUTPUT);
  pinMode(sm1.pin, OUTPUT);
  pinMode(sm2.dirpin, OUTPUT);
  pinMode(sm2.pin, OUTPUT);
  pinMode(em1.dirpin, OUTPUT);
  pinMode(em1.pin, OUTPUT);
  pinMode(em2.pin, OUTPUT);
  pinMode(em2.dirpin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(relayPin, OUTPUT);

  hm1.attach(2);
  grip.attach(3);
  hm1.write(hm1pos);
  grip.write(gripperpos);
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------LOOP------------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void loop()
{
  homebut = ButtonRead(HOME_BUTTON);
  switch (homebut)
  {
  case 1:
    Serial.println("Going to Home Position");
    // gotoHome();
    testGoToHome();
    break;
    break;
  case 2:
    Serial.println("Setting new Home");
    setHomePos(sm1.step, sm2.step, em1.step, em2.step, upDownCount);
    break;
  default:
    break;
  }

  drinkbut = ButtonRead(DRINK_BUTTON);
  switch (drinkbut)
  {
  case 1:
    Serial.println("Going to Home Position");
    // DrinkingPos();
    testGoToDrink();
    break;
    break;
  case 2:
    Serial.println("Setting new Home");
    setDrinkingPos(sm1.step, sm2.step, em1.step, em2.step, upDownCount);
    break;
  default:
    break;
  }

  buttoncheck();

  y = analogRead(A1);
  x = analogRead(A0);
  if(y > 850 || y < 300){
    speed2 = 400;
  }
  else{
    speed2 = 800;
  }

  if (modeFlag)
  {
    delay(100);
    mode1 = digitalRead(39);
    if (mode1 == modeFlag)
    {
      mode = !mode;
      modeFlag = 0;
    }
    else
    {
      modeFlag = 0;
    }
  }

  if (powerFlag)
  {
    delay(100);
    power1 = digitalRead(43);
    if (power1 == powerFlag)
    {
      power = !power;
      powerFlag = 0;
    }
    else
    {
      powerFlag = 0;
    }
  }

  if (power)
  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(sm2.dirpin, LOW);
    while (sm2Count < 4750)
    {
      digitalWrite(sm2.pin, HIGH);
      delayMicroseconds(600);
      digitalWrite(sm2.pin, LOW);
      delayMicroseconds(600);
      sm2Count++;
    }
    while (timer < 1)
    {
      delay(12000);
      timer++;
    }
    hm1.write(hm1pos);
    v = 0;
    r = 0;
    t = 0;
    w = 0;
  }
  else
  {
    powerOff();
    timer = 0;
    sm2Count = 0;
  }

  if (mode)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, HIGH);

    em2move();
    if (digitalRead(joystick_button))
    {
      hm1move();
    }
    else
    {
      grip1();
    }
  }

  else
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);

    leftRight();
    if (digitalRead(joystick_button))
    {
      maxReach();
    }
    else
    {
      fwdBk();
    }
  }
  // // if (digitalRead(43)){
  //   doorOpen();
  // }
  // Serial.print("x: ");Serial.print(x);
  // Serial.print("  y: ");Serial.println(y);
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------CUSTOM FUNCTION BLOCKS -------------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void makeStep(int motor, int speed)
{
  digitalWrite(motor, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor, LOW);
  delayMicroseconds(speed);
}

void makeStep2(int motor1, int motor2, int speed)
{
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  delayMicroseconds(speed);
}

void makeStep3(int motor1, int motor2, int motor3, int speed)
{
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  delayMicroseconds(speed);
}

void makeStep4(int motor1, int motor2, int motor3, int motor4, int speed)
{
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, HIGH);
  delayMicroseconds(speed);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);
  delayMicroseconds(speed);
}

void makeSteps(int motor, int speed, int steps)
{
  int x = 1;
  while (x <= steps)
  {
    makeStep(motor, speed);
    x++;
  }
}

void makeSteps3(int motor1, int motor2, int motor3, int speed, int steps)
{
  int x = 1;
  while (x <= steps)
  {
    makeStep3(motor1, motor2, motor3, speed);
    x++;
  }
}

void makeSteps4(int motor1, int motor2, int motor3, int motor4, int speed, int steps)
{
  int x = 1;
  while (x <= steps)
  {
    makeStep4(motor1, motor2, motor3, motor4, speed);
    x++;
  }
}

void makeScaledStep(int motor1, int motor2, int speed, float scale)
{
  int step1 = 5;
  int step2 = 5;
  step2 = step1 * scale;
  makeSteps(motor1, speed, step1);
  makeSteps(motor2, speed, step2);
}

void makeScaledStep2(int motor1, int motor2, int speed, int speed2, float scale)
{
  int step1 = 10;
  int step2 = 10;
  step2 = step1 * scale;
  makeSteps(motor1, speed, step1);
  makeSteps(motor2, speed2, step2);
}

void fwdBk()
{
  if (y < 450 && sm2.step <= 1718 && em1.step <= 5790)
  {
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, LOW);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 0.9);
    sm2.step += 10;
    em1.step += 9;
    //    if (em2.step >-1072 && em2.step < 1072){
    //        hm1.write(hm1pos);
    //        hm1pos-= 0.008;
    //        delayMicroseconds(100);
    //    }
  }
  else if (y < 450 && sm2.step >= 1718 && em1.step <= 5790)
  {
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, LOW);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 1.1);
    sm2.step += 10;
    em1.step += 10;
    //    if (em2.step >-1072 && em2.step < 1072){
    //        hm1.write(hm1pos);
    //        hm1pos-= 0.008;
    //        delayMicroseconds(100);
    //    }
  }
  else if (y > 600 && sm2.step <= 1718)
  {
    digitalWrite(sm2.dirpin, HIGH);
    digitalWrite(em1.dirpin, HIGH);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 0.9);
    sm2.step -= 10;
    em1.step -= 9;
    //    if (em2.step >-1072 && em2.step < 1072){
    //        hm1.write(hm1pos);
    //        hm1pos+= 0.008;
    //        delayMicroseconds(100);
    //    }
  }
  else if (y > 600 && sm2.step >= 1718)
  {
    digitalWrite(sm2.dirpin, HIGH);
    digitalWrite(em1.dirpin, HIGH);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 1.1);
    sm2.step -= 10;
    em1.step -= 10;
    //    if (em2.step >-1072 && em2.step < 1072){
    //        hm1.write(hm1pos);
    //        hm1pos+= 0.008;
    //        delayMicroseconds(100);
    //    }
  }
  // else if (y < 450 && em1.step >= 6136){
  //   digitalWrite(em1.dirpin, HIGH);
  //   makeStep(em1.pin, 400);
  // }
}
void upDown()
{
  if (y < 450 && upDownCount >= 0)
  {
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step += 10;
    sm2.step += 3;
    upDownCount += 5;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos -= 0.12;
      delayMicroseconds(100);
    }
  }

  else if (y < 450 && upDownCount <= 0)
  {
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step += 10;
    sm2.step -= 3;
    upDownCount += 5;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos -= 0.12;
      delayMicroseconds(100);
    }
  }

  if (y > 600 && upDownCount >= 0)
  {
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step -= 10;
    sm2.step -= 3;
    upDownCount -= 5;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos += 0.12;
      delayMicroseconds(100);
    }
  }

  else if (y > 600 && upDownCount <= 0)
  {
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step -= 10;
    sm2.step += 3;
    upDownCount -= 5;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos += 0.12;
      delayMicroseconds(100);
    }
  }
}
void down()
{
  if (y > 600 && upDownCount >= 0)
  {
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step -= 10;
    sm2.step -= 3;
    upDownCount -= 1;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos += 0.12;
      delayMicroseconds(100);
    }
  }

  else if (y > 600 && upDownCount <= 0)
  {
    digitalWrite(em1.dirpin, HIGH);
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.3);
    em1.step -= 10;
    sm2.step += 3;
    upDownCount -= 5;
    if (em2.step > -1072 && em2.step < 1072)
    {
      hm1.write(hm1pos);
      hm1pos += 0.12;
      delayMicroseconds(100);
    }
  }
}

void maxReach()
{
  if (em1.step <= 6670)
  {
    upDown();
  }
  else if (em1.step >= 6670)
  {
    down();
  }
}

void readVal()
{
  int j = 0;
  while (Wire.available())
  {
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
  else
  {
    grip.write(90);
  }
}

void hm1move()
{
  if (y > 600)
  {
    if (hm1pos > 20 && hm1pos < 120 || hm1pos >= 120)
    {
      hm1pos -= 0.5;
      hm1.write(hm1pos);
      delay(10);
    }
  }
  else if (y < 450)
  {
    if (hm1pos > 20 && hm1pos < 120 || hm1pos <= 20)
    {
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
    em2.step++;
  }
  else if (x > 600)
  {
    digitalWrite(em2.dirpin, HIGH);
    makeStep(em2.pin, 600);
    em2.step--;
  }
}

void buttoncheck()
{
  modeFlag = digitalRead(39);
  powerFlag = digitalRead(43);
}

// void motorHome(int pin, int steps, int home){
//   int correction,moveservo, moveservo2, dir1;
//   if(steps >= home){
//     switch(pin){
//       case 13:
//         digitalWrite(sm1.dirpin, HIGH);
//         correction = steps - home;
//         Serial.print("SM1 moving -"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       case 8:
//         digitalWrite(sm2.dirpin, LOW);
//         correction = steps - home;
//         Serial.print("SM2 moving -"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 1;
//         dir = 0;
//         break;
//       case 10:
//         digitalWrite(em1.dirpin, HIGH);
//         correction = steps - home;
//         upDownCount = 0;
//         Serial.print("EM1 moving -"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 1;
//         moveservo2 = 0;
//         dir = 0;
//         break;
//       case 22:
//         digitalWrite(em2.dirpin, HIGH);
//         correction = steps - home;
//         Serial.print("EM2 moving -"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       default:
//         Serial.println("Invalid Pin Selected.");
//         break;
//     }
//   }
//   else if(steps <= home){
//     switch(pin){
//       case 13:
//         digitalWrite(sm1.dirpin, LOW);
//         correction = home - steps;
//         Serial.print("SM1 moving +"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       case 8:
//         digitalWrite(sm2.dirpin, HIGH);
//         correction = home - steps;
//         Serial.print("SM2 moving +"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 1;
//         dir = 1;
//         break;
//       case 10:
//         digitalWrite(em1.dirpin, LOW);
//         correction = home - steps;
//         upDownCount = 0;
//         Serial.print("EM1 moving +"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 1;
//         moveservo2 = 0;
//         dir = 1;
//         break;
//       case 22:
//         digitalWrite(em2.dirpin, LOW);
//         correction = home - steps;
//         Serial.print("EM2 moving +"); Serial.print(correction); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       default :
//         Serial.println("Invalid Pin Argument");
//         moveservo =0;
//         moveservo2 = 0;
//         break;
//       }
//   }
//   else{}
//     if(moveservo){
//       makeServoSteps(pin, 400, correction, dir);
//     }
//     else if (moveservo2){
//       makeServoSteps2(pin, 400,correction,dir);
//     }
//     else{
//       makeSteps(pin, 400, correction);
//     }
// }

// void gotoHome(){
//   motorHome(sm2.pin, sm2.step, sm2.home);
//   sm2.step = sm2.home;
//   motorHome(em1.pin, em1.step, em1.home);
//   em1.step = em1.home;
//   motorHome(sm1.pin, sm1.step, sm1.home);
//   sm1.step = sm1.home;
//   motorHome(em2.pin, em2.step, em2.home);
//   em2.step = em2.home;
//   upDownCount = upDownCountHome;
// }

void mPrint(String xyz)
{
  if (xyz == "steps")
  {
    Serial.println("\n SM1 SM2 EM1 EM2 ");
    Serial.print("  ");
    Serial.print(sm1.step);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(sm2.step);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(em1.step);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(em2.step);
    Serial.println(" ");
  }
  else if (xyz == "home")
  {
    Serial.println("\n SM1 SM2 EM1 EM2 ");
    Serial.print("  ");
    Serial.print(sm1.home);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(sm2.home);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(em1.home);
    Serial.print(" ");
    Serial.print("  ");
    Serial.print(em2.home);
    Serial.println(" ");
  }
  else
  {
    //Serial.println("\nInvalid Argument to mPrint");
  }
}

void setHomePos(int x, int y, int z, int a, int b)
{
  sm1.home = x;
  setHome(sm1.home, SM1Address);
  sm2.home = y;
  setHome(sm2.home, SM2Address);
  em1.home = z;
  setHome(em1.home, EM1Address);
  em2.home = a;
  setHome(em2.home, EM2Address);
  upDownCountHome = b;
  setHome(upDownCountHome, upDownCountAddress);
}

int ButtonRead(int button)
{
  count = 0;
  while (digitalRead(button))
  {
    delay(10);
    count++;
  }
  if (count > 100)
  {
    return 2;
  }
  else if (count > 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// void motorHome1(int pin, int steps, int home2){
//   int correction1, moveservo, moveservo2,dir1;
//   if(steps > home2){
//     switch(pin){
//       case 13:
//         digitalWrite(sm1.dirpin, HIGH);
//         correction1 = steps - home2;
//         Serial.print("SM1 moving -"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       case 8:
//         digitalWrite(sm2.dirpin, LOW);
//         correction1 = steps - home2;
//         Serial.print("SM2 moving -"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 1;
//         dir = 0;
//         break;
//       case 10:
//         digitalWrite(em1.dirpin, HIGH);
//         correction1 = steps - home2;
//         upDownCount = 0;
//         Serial.print("EM1 moving -"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 1;
//         moveservo2 = 0;
//         dir = 0;
//         break;
//       case 22:
//         digitalWrite(em2.dirpin, HIGH);
//         correction1 = steps - home2;
//         Serial.print("EM2 moving -"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       default:
//         Serial.println("Invalid Pin Selected.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//     }
//   }
//   else if(steps < home2){
//     switch(pin){
//       case 13:
//         digitalWrite(sm1.dirpin, LOW);
//         correction1 = home2 - steps;
//         Serial.print("SM1 moving +"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       case 8:
//         digitalWrite(sm2.dirpin, HIGH);
//         correction1 = home2 - steps;
//         Serial.print("SM2 moving +"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 1;
//         dir = 1;
//         break;
//       case 10:
//         digitalWrite(em1.dirpin, LOW);
//         correction1 = home2 - steps;
//         upDownCount = 0;
//         Serial.print("EM1 moving +"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 1;
//         moveservo2 = 0;
//         dir = 1;
//         break;
//       case 22:
//         digitalWrite(em2.dirpin, LOW);
//         correction1 = home2 - steps;
//         Serial.print("EM2 moving +"); Serial.print(correction1); Serial.println(" steps.");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       default :
//         Serial.println("Invalid Pin Argument");
//         moveservo = 0;
//         moveservo2 = 0;
//         break;
//       }
//   }
//   else{}
//     if(moveservo){
//       makeServoSteps(pin, 400, correction1, dir);
//     }
//     else if (moveservo2){
//       makeServoSteps2(pin,400,correction1,dir);
//     }
//     else{
//       makeSteps(pin, 400, correction1);
//     }
// }
// void DrinkingPos(){
//   motorHome1(sm2.pin, sm2.step, sm2.home2);
//   sm2.step = sm2.home2;
//   motorHome1(em1.pin, em1.step, em1.home2);
//   em1.step = em1.home2;
//   motorHome1(sm1.pin, sm1.step, sm1.home2);
//   sm1.step = sm1.home2;
//   motorHome1(em2.pin, em2.step, em2.home2);
//   em2.step = em2.home2;
//   upDownCount = upDownCountHome2;
// }

void setDrinkingPos(int b, int c, int d, int e, int f)
{
  sm1.home2 = b;
  setHome(sm1.home2, SM1Address2);

  sm2.home2 = c;
  setHome(sm2.home2, SM2Address2);

  em1.home2 = d;
  setHome(em1.home2, EM1Address2);

  em2.home2 = e;
  setHome(em2.home2, EM2Address2);

  upDownCountHome2 = f;
  setHome(upDownCountHome2, upDownCountAddress2);
}

void setHome(int home, int address)
{
  int x = home / 255;
  EEPROM.write(address, x);
  int y = home % 255;
  EEPROM.write(address + 1, y);
}

void leftRight()
{
  (x > 850 || x < 300) ? speed = 500: speed = 1000;
  if (x > 600)
  {
    digitalWrite(sm1.dirpin, HIGH);
    makeStep(sm1.pin, speed);
    sm1.step--;
    if (y > 600)
    {
      makeSteps(sm1.pin, 900, 5);
      sm1.step -= 5;
    }
    else if (y < 450)
    {
      makeSteps(sm1.pin, 900, 5);
      sm1.step -= 5;
    }
  }
  else if (x < 450)
  {
    digitalWrite(sm1.dirpin, LOW);
    makeStep(sm1.pin, speed);
    sm1.step++;
    if (y < 450)
    {
      makeSteps(sm1.pin, 900, 5);
      sm1.step += 5;
    }
    if (y > 600)
    {
      makeSteps(sm1.pin, 900, 5);
      sm1.step += 5;
    }
  }
}

void makeServoSteps(int motor, int speed, int correction, int direction)
{
  int i = 0;
  if (i < correction)
  {
    if (direction)
    {
      hm1pos += 0.01;
    }
    else
    {
      hm1pos -= 0.01;
    }
    // makeStep(motor, speed);
    hm1.write(hm1pos);
    i++;
  }
}

void makeServoSteps2(int motor, int speed, int correction, int direction)
{
  int i = 0;
  if (i < correction)
  {
    if (direction)
    {
      hm1pos -= 0.01;
    }
    else
    {
      hm1pos -= 0.01;
    }
    // makeStep(motor, speed);
    // makeStep(motor2, speed2);
    hm1.write(hm1pos);
    i++;
  }
}

void doorOpen()
{
  digitalWrite(em2.dirpin, LOW);
  digitalWrite(em1.dirpin, HIGH);
  digitalWrite(sm1.dirpin, HIGH);
  makeSteps3(em2.pin, em1.pin, sm1.pin, 600, 500);
  em1.step -= 500;
  em2.step += 500;
  sm1.step -= 500;
}

void powerOff()
{
  hm1.write(120);

  if (sm1.step <= 0)
  {
    powerCorrection = abs(sm1.step - sm1.powerHome);
    digitalWrite(sm1.dirpin, LOW);
  }
  else if (sm1.step >= 0)
  {
    powerCorrection = abs(sm1.powerHome - sm1.step);
    digitalWrite(sm1.dirpin, HIGH);
  }

  if (em2.step <= 0)
  {
    powerCorrection4 = abs(em2.step - em2.powerHome);
    digitalWrite(em2.dirpin, LOW);
  }
  else if (em2.step > 0)
  {
    powerCorrection4 = abs(em2.powerHome - em2.step);
    digitalWrite(em2.dirpin, HIGH);
  }

  if (sm2.step <= 0)
  {
    powerCorrection2 = abs(sm2.step - sm2.powerHome);
    digitalWrite(sm2.dirpin, HIGH);
  }
  else if (sm2.step > 0)
  {
    powerCorrection2 = abs(sm2.powerHome - sm2.step);
    digitalWrite(sm2.dirpin, HIGH);
  }

  if (em1.step <= 0)
  {
    powerCorrection3 = abs(em1.step - em1.powerHome);
    digitalWrite(em1.dirpin, HIGH);
  }
  else if (em1.step > 0)
  {
    powerCorrection3 = abs(em1.powerHome - em1.step);
    digitalWrite(em1.dirpin, HIGH);
  }

  totalPower = max(max(powerCorrection, powerCorrection2), max(powerCorrection3, powerCorrection4));

  while (t < totalPower)
  {
    if (t < powerCorrection)
      digitalWrite(sm1.pin, HIGH);
    if (t < powerCorrection4)
      digitalWrite(em2.pin, HIGH);
    if (t < powerCorrection2)
      digitalWrite(sm2.pin, HIGH);
    if (t < powerCorrection3)
      digitalWrite(em1.pin, HIGH);
    delayMicroseconds(400);
    if (t < powerCorrection)
      digitalWrite(sm1.pin, LOW);
    if (t < powerCorrection4)
      digitalWrite(em2.pin, LOW);
    if (t < powerCorrection2)
      digitalWrite(sm2.pin, LOW);
    if (t < powerCorrection3)
      digitalWrite(em1.pin, LOW);
    delayMicroseconds(400);
    t++;
  }

  sm1.step = 0;
  sm2.step = 0;
  em1.step = 0;
  em2.step = 0;
  upDownCount = 0;

  digitalWrite(relayPin, LOW);
}

void testGoToHome()
{
  int servoDirection, servoDirection2, servoRequired;
  if (sm1.step <= sm1.home)
  {
    testCorrection = abs(sm1.step - sm1.home);
    digitalWrite(sm1.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }
  else if (sm1.step >= sm1.home)
  {
    testCorrection = abs(sm1.home - sm1.step);
    digitalWrite(sm1.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }

  if (em2.step <= em2.home)
  {
    testCorrection4 = abs(em2.step - em2.home);
    digitalWrite(em2.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }
  else if (em2.step > em2.home)
  {
    testCorrection4 = abs(em2.home - em2.step);
    digitalWrite(em2.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }

  if (sm2.step <= sm2.home)
  {
    testCorrection2 = abs(sm2.step - sm2.home);
    digitalWrite(sm2.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  else if (sm2.step > sm2.home)
  {
    testCorrection2 = abs(sm2.home - sm2.step);
    digitalWrite(sm2.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 1;
    servoRequired = 1;
  }

  if (em1.step <= em1.home)
  {
    testCorrection3 = abs(em1.step - em1.home);
    digitalWrite(em1.dirpin, LOW);
    servoDirection = 1;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  else if (em1.step > em1.home)
  {
    testCorrection3 = abs(em1.home - em1.step);
    digitalWrite(em1.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  totalCorrection = max(testCorrection2, max(testCorrection3, testCorrection4));
  while (w < totalCorrection)
  {
    // if (w < testCorrection) digitalWrite(sm1.pin, HIGH);
    if (w < testCorrection4)
      digitalWrite(em2.pin, HIGH);
    if (w < testCorrection2)
      digitalWrite(sm2.pin, HIGH);
    if (w < testCorrection3)
      digitalWrite(em1.pin, HIGH);
    delayMicroseconds(400);
    // if (w < testCorrection) digitalWrite(sm1.pin, LOW);
    if (w < testCorrection4)
      digitalWrite(em2.pin, LOW);
    if (w < testCorrection2)
      digitalWrite(sm2.pin, LOW);
    if (w < testCorrection3)
      digitalWrite(em1.pin, LOW);
    delayMicroseconds(400);
    if (w < testCorrection3)
    {
      if (servoRequired)
      {
        if (servoDirection)
        {
          hm1pos -= 0.012;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos += 0.012;
          hm1.write(hm1pos);
        }
      }
    }

    if (w < testCorrection2)
    {
      if (servoRequired)
      {
        if (servoDirection2)
        {
          hm1pos -= 0.012;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos -= 0.012;
          hm1.write(hm1pos);
        }
      }
    }

    // Serial.print("  totalCorrection: ");Serial.print(totalCorrection);
    // Serial.print("  testCorrection2: ");Serial.print(testCorrection2);
    // Serial.print("  testCorrection3: ");Serial.println(testCorrection3);
    w++;
    // r++;
  }

  while (t < testCorrection)
  {
    digitalWrite(sm1.pin, HIGH);
    delayMicroseconds(400);
    digitalWrite(sm1.pin, LOW);
    delayMicroseconds(400);
    t++;
  }
  sm1.step = sm1.home;
  sm2.step = sm2.home;
  em1.step = em1.home;
  em2.step = em2.home;
  upDownCount = upDownCountHome;
}

void testGoToDrink()
{
  int servoDirection, servoDirection2, servoRequired;
  if (sm1.step <= sm1.home2)
  {
    drinkCorrection = abs(sm1.step - sm1.home2);
    digitalWrite(sm1.dirpin, LOW);
    servoRequired = 0;
    servoDirection = 0;
    servoDirection2 = 0;
  }
  else if (sm1.step >= sm1.home2)
  {
    drinkCorrection = abs(sm1.home2 - sm1.step);
    digitalWrite(sm1.dirpin, HIGH);
    servoRequired = 0;
    servoDirection = 0;
    servoDirection2 = 0;
  }

  if (em2.step <= em2.home2)
  {
    drinkCorrection4 = abs(em2.step - em2.home2);
    digitalWrite(em2.dirpin, LOW);
    servoRequired = 0;
    servoDirection = 0;
    servoDirection2 = 0;
  }
  else if (em2.step > em2.home2)
  {
    drinkCorrection4 = abs(em2.home2 - em2.step);
    digitalWrite(em2.dirpin, HIGH);
    servoRequired = 0;
    servoDirection = 0;
    servoDirection2 = 0;
  }

  if (sm2.step <= sm2.home2)
  {
    drinkCorrection2 = abs(sm2.step - sm2.home2);
    digitalWrite(sm2.dirpin, LOW);
    servoRequired = 1;
    servoDirection2 = 0;
    servoDirection = 0;
  }
  else if (sm2.step > sm2.home2)
  {
    drinkCorrection2 = abs(sm2.home2 - sm2.step);
    digitalWrite(sm2.dirpin, HIGH);
    servoRequired = 0;
    servoDirection = 0;
    servoDirection2 = 1;
  }

  if (em1.step <= em1.home2)
  {
    drinkCorrection3 = abs(em1.step - em1.home2);
    digitalWrite(em1.dirpin, LOW);
    servoRequired = 1;
    servoDirection = 1;
    servoDirection2 = 0;
  }
  else if (em1.step > em1.home2)
  {
    drinkCorrection3 = abs(em1.home2 - em1.step);
    digitalWrite(em1.dirpin, HIGH);
    servoRequired = 1;
    servoDirection = 0;
    servoDirection2 = 0;
  }

  drinkTotalCorrection = max(drinkCorrection2, max(drinkCorrection3, drinkCorrection4));

  while (w < drinkTotalCorrection)
  {
    // if (w < drinkCorrection) digitalWrite(sm1.pin, HIGH);
    if (w < drinkCorrection4)
      digitalWrite(em2.pin, HIGH);
    if (w < drinkCorrection2)
      digitalWrite(sm2.pin, HIGH);
    if (w < drinkCorrection3)
      digitalWrite(em1.pin, HIGH);
    delayMicroseconds(400);
    // if (w < drinkCorrection) digitalWrite(sm1.pin, LOW);
    if (w < drinkCorrection4)
      digitalWrite(em2.pin, LOW);
    if (w < drinkCorrection2)
      digitalWrite(sm2.pin, LOW);
    if (w < drinkCorrection3)
      digitalWrite(em1.pin, LOW);
    delayMicroseconds(400);
    w++;
    if (w < drinkCorrection4)
    {
      if (servoRequired)
      {
        if (servoDirection)
        {
          hm1pos -= 0.012;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos += 0.012;
          hm1.write(hm1pos);
        }
      }
    }

    if (w < testCorrection2)
    {
      if (servoRequired)
      {
        if (servoDirection2)
        {
          hm1pos -= 0.012;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos += 0.012;
          hm1.write(hm1pos);
        }
      }
    }
  }
  while (t < drinkCorrection)
  {
    digitalWrite(sm1.pin, HIGH);
    delayMicroseconds(400);
    digitalWrite(sm1.pin, LOW);
    delayMicroseconds(400);
    t++;
  }
  sm1.step = sm1.home2;
  sm2.step = sm2.home2;
  em1.step = em1.home2;
  em2.step = em2.home2;
  upDownCount = upDownCountHome2;

  // Serial.print("drinkCorrection");Serial.println(drinkCorrection);
}

