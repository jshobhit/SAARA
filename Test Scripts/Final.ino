#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

LSM9DS1 imu;

#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

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

int relayPin = 37;

int dir;
float upDownCount;
int upDownCountHome, upDownCountHome2;
int em2step = 1171;
int em2home = 1171;

int powerCorrection, powerCorrection2, powerCorrection3, powerCorrection4, totalPower;
int homeCorrection, homeCorrection2, homeCorrection3, homeCorrection4, totalhomeCorrection;
int drinkCorrection, drinkCorrection2, drinkCorrection3, drinkCorrection4, drinkTotalCorrection;

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
int mode_button = 45;
int joystick_button = 44;

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
int accelerometer = 0;
int sm2Count = 0;
int em1Count = 0; 
volatile float y_accel, y_accel2;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1)
      ;
  }

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
  // digitalWrite(sm2.dirpin, HIGH);
  // while (sm2Count < 1800)
  // {
  //   digitalWrite(sm2.pin, HIGH);
  //   delayMicroseconds(600);
  //   digitalWrite(sm2.pin, LOW);
  //   delayMicroseconds(600);
  //   sm2Count++;
  // } 
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------LOOP------------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void loop()
{
  while (accelerometer < 5000){
    imu.readAccel();
    y_accel2 = imu.calcAccel(imu.ay);
    Serial.print("  y_accel: ");Serial.println(y_accel2); 
    while (y_accel2 > -0.29){
      digitalWrite(em1.dirpin, HIGH); 
      makeSteps(em1.pin, 400, 5);
      y_accel2 -= 1;
    }
    accelerometer++;
  }  
  if (accelerometer == 5000){ 
    digitalWrite(em1.dirpin, LOW);
    digitalWrite(sm2.dirpin, HIGH);
    while (em1Count < 3100)
    {
      if (em1Count < 1800) digitalWrite(sm2.pin, HIGH);
      if (em1Count < 3100) digitalWrite(em1.pin, HIGH);
      delayMicroseconds(600);
      if (em1Count < 1800) digitalWrite(sm2.pin, LOW);
      if (em1Count < 3100) digitalWrite(em1.pin, LOW);
      delayMicroseconds(600);
      em1Count++;
    }     
}


  // while (accelerometer < 7000){
  //   imu.readAccel();
  //   y_accel = imu.calcAccel(imu.ay);
  //   Serial.print("  y_accel: ");Serial.println(y_accel); 
  //   while (y_accel > 0.7){
  //     digitalWrite(sm2.dirpin, LOW); 
  //     makeSteps(sm2.pin, 600, 5);
  //     y_accel -= 1;
  //   }
  //   accelerometer++;
  // }
  
  // if (accelerometer == 7000){ 
  //   digitalWrite(sm2.dirpin, HIGH);
  //   while (sm2Count < 1800)
  //   {
  //     digitalWrite(sm2.pin, HIGH);
  //     delayMicroseconds(600);
  //     digitalWrite(sm2.pin, LOW);
  //     delayMicroseconds(600);
  //     sm2Count++;
  //   }     
  // }
  homebut = ButtonRead(HOME_BUTTON);
  switch (homebut)
  {
  case 1:
    Serial.println("Going to Home Position");
    goToHome();
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
    goToDrink();
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
    mode1 = digitalRead(45);
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
    power1 = digitalRead(37);
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

  // if (power)
  // {
  //   digitalWrite(relayPin, HIGH);
  //   digitalWrite(sm2.dirpin, LOW);
  //   while (sm2Count < 4750)
  //   {
  //     digitalWrite(sm2.pin, HIGH);
  //     delayMicroseconds(600);
  //     digitalWrite(sm2.pin, LOW);
  //     delayMicroseconds(600);
  //     sm2Count++;
  //   }
  //   while (timer < 1)
  //   {
  //     delay(4000);
  //     timer++;
  //   }
  //   hm1.write(hm1pos);
  //   v = 0;
  //   r = 0;
  //   t = 0;
  //   w = 0;
  // }
  // else
  // {
  //   powerOff();
  //   timer = 0;
  //   sm2Count = 0;
  // }

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
  // Serial.print("sm2 step: ");Serial.print(sm2.step);
  // Serial.print("  em1 step: ");Serial.println(em1.step); 
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
  if (y < 450 && sm2.step <= 1680 && em1.step <= 6000)
  {
    digitalWrite(sm2.dirpin, HIGH);
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
  else if (y < 450 && sm2.step >= 1680 && em1.step <= 6000)
  {
    digitalWrite(sm2.dirpin, HIGH);
    digitalWrite(em1.dirpin, LOW);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 1.1);
    sm2.step += 10;
    em1.step += 11;
    //    if (em2.step >-1072 && em2.step < 1072){
    //        hm1.write(hm1pos);
    //        hm1pos-= 0.008;
    //        delayMicroseconds(100);
    //    }
  }
  else if (y > 600 && sm2.step <= 1680)
  {
    digitalWrite(sm2.dirpin, LOW);
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
  else if (y > 600 && sm2.step >= 1680)
  {
    digitalWrite(sm2.dirpin, LOW);
    digitalWrite(em1.dirpin, HIGH);
    makeScaledStep2(sm2.pin, em1.pin, speed2*2, speed2, 1.1);
    sm2.step -= 10;
    em1.step -= 11;
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
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step += 10;
    sm2.step += 2;
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
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step += 10;
    sm2.step -= 2;
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
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step -= 10;
    sm2.step -= 2;
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
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step -= 10;
    sm2.step += 2;
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
    digitalWrite(sm2.dirpin, LOW);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step -= 10;
    sm2.step -= 2;
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
    digitalWrite(sm2.dirpin, HIGH);
    makeScaledStep2(em1.pin, sm2.pin, speed2, speed2*2, 0.2);
    em1.step -= 10;
    sm2.step += 2;
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
  if (em1.step <= 6000)
  {
    upDown();
  }
  else if (em1.step >= 6000)
  {
    down();
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
  modeFlag = digitalRead(45);
  powerFlag = digitalRead(43);
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
    digitalWrite(sm2.dirpin, LOW);
  }
  else if (sm2.step > 0)
  {
    powerCorrection2 = abs(sm2.powerHome - sm2.step);
    digitalWrite(sm2.dirpin, LOW);
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
  t = 0; 
  digitalWrite(relayPin, LOW);
}

void goToHome()
{
  int servoDirection, servoDirection2, servoRequired;
  if (sm1.step <= sm1.home)
  {
    homeCorrection = abs(sm1.step - sm1.home);
    digitalWrite(sm1.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }
  else if (sm1.step >= sm1.home)
  {
    homeCorrection = abs(sm1.home - sm1.step);
    digitalWrite(sm1.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }

  if (em2.step <= em2.home)
  {
    homeCorrection4 = abs(em2.step - em2.home);
    digitalWrite(em2.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }
  else if (em2.step > em2.home)
  {
    homeCorrection4 = abs(em2.home - em2.step);
    digitalWrite(em2.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 0;
  }

  if (sm2.step <= sm2.home)
  {
    homeCorrection2 = abs(sm2.step - sm2.home);
    digitalWrite(sm2.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  else if (sm2.step > sm2.home)
  {
    homeCorrection2 = abs(sm2.home - sm2.step);
    digitalWrite(sm2.dirpin, LOW);
    servoDirection = 0;
    servoDirection2 = 1;
    servoRequired = 1;
  }

  if (em1.step <= em1.home)
  {
    homeCorrection3 = abs(em1.step - em1.home);
    digitalWrite(em1.dirpin, LOW);
    servoDirection = 1;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  else if (em1.step > em1.home)
  {
    homeCorrection3 = abs(em1.home - em1.step);
    digitalWrite(em1.dirpin, HIGH);
    servoDirection = 0;
    servoDirection2 = 0;
    servoRequired = 1;
  }
  totalhomeCorrection = max(homeCorrection2, max(homeCorrection3, homeCorrection4));
  while (w < totalhomeCorrection)
  {
    if (w < homeCorrection4)
      digitalWrite(em2.pin, HIGH);
    if (w < homeCorrection2)
      digitalWrite(sm2.pin, HIGH);
    if (w < homeCorrection3)
      digitalWrite(em1.pin, HIGH);
    delayMicroseconds(400);
    if (w < homeCorrection4)
      digitalWrite(em2.pin, LOW);
    if (w < homeCorrection2)
      digitalWrite(sm2.pin, LOW);
    if (w < homeCorrection3)
      digitalWrite(em1.pin, LOW);
    delayMicroseconds(400);
    if (w < homeCorrection3)
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

    if (w < homeCorrection2)
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
    w++;
  }

  while (t < homeCorrection)
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
  w = 0;
  t = 0; 
}

void goToDrink()
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
    digitalWrite(sm2.dirpin, HIGH);
    servoRequired = 1;
    servoDirection2 = 0;
    servoDirection = 0;
  }
  else if (sm2.step > sm2.home2)
  {
    drinkCorrection2 = abs(sm2.home2 - sm2.step);
    digitalWrite(sm2.dirpin, LOW);
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
    if (w < drinkCorrection4)
      digitalWrite(em2.pin, HIGH);
    if (w < drinkCorrection2)
      digitalWrite(sm2.pin, HIGH);
    if (w < drinkCorrection3)
      digitalWrite(em1.pin, HIGH);
    delayMicroseconds(400);
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
          hm1pos -= 0.014;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos += 0.014;
          hm1.write(hm1pos);
        }
      }
    }

    if (w < drinkCorrection2)
    {
      if (servoRequired)
      {
        if (servoDirection2)
        {
          hm1pos -= 0.014;
          hm1.write(hm1pos);
        }
        else
        {
          hm1pos += 0.014;
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
  w = 0; 
  t = 0;
}

