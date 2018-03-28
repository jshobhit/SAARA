
#include<Arm.h>                           
#include<Arm_pump.h>                       

Arm_pump *p1 = new Arm_pump(); 

Serial_arm sa; 
Steer rotate(3,&Serial1);                      
Steer hm1(4, &Serial1);
Steer em2(5, &Serial1);
Steer grip(6, &Serial1);

float x,y,z,t;
int posr, posh, pose, posg; // Individual Position variables for all the steer arms.
bool torq;
void setup() {
  MyArm.begin(USB_SER);
  Serial.begin(115200);
  MyArm.position_init();
  delay(1000);

  x = 0;
  y = 136;
  z = 250.5;
  t = 1000;
  posh = 2000;
  posr = 2000;
  pose = 2000;
  posg = 1550;
	MyArm.move_to_position((double)x,y,z,t);  
  hm1.Set_Steer_position_runtime(posh, t);
  rotate.Set_Steer_position_runtime(posr, t);
  em2.Set_Steer_position_runtime(pose, t);
  grip.Set_Steer_position_runtime(posg, t);
  MyArm.Set_Arm_Torque_On();
  torq = 1;

}

bool mode = 0;
bool pump = 0;

void torqueOn(){
	MyArm.position_init();
  MyArm.Set_Arm_Torque_On();
}
void loop() {
  
  char a = Serial.read();
  switch(a){
  	case 'm': // Mode Indicator Button
  		(mode) ? Serial.write("Q"):Serial.write("q") ;
  		mode = !mode;
  		break;
  	case 't':	// Torque On/Off Button
  		(torq) ? torqueOn() : MyArm.Set_Arm_Torque_Off();
  		torq = !torq;
  		break;
  	case 'p': // Pump On/Off Button
  		pump ? p1->pump_on() : p1->pump_off();
  		pump = !pump;
  		break;
  	case 'o': // Position show
  		Serial.write('y');
  		Serial.write(5);
  		break;

    case 'u':
    	if(mode){
    		//y 	+= 0.2;
    		y += 0.5;
    		//z += 0.5;
    	}
    	else{    		
      	z += 0.5;
      	//y += 0.5;	
    	}
      //z += 0.2;
	  	MyArm.move_to_position((double)x,y,z,t);  
      Serial.write("R");
      break;
    case 'd':
    	if(mode){
        y -= 0.5;
        //z += 0.2;
    	}
    	else{
    		z -= 0.5;
    		//y += 0.2;
    	}      
      MyArm.move_to_position((double)x,y,z,t);  
      Serial.write("G");
      break;
    case 'l':
      x -= 0.5;
      MyArm.move_to_position((double)x,y,z,t);  
      Serial.write("Y");
      break;
    case 'r':
      x += 0.5;
      MyArm.move_to_position((double)x,y,z,t);  
      Serial.write("O");
      break;
    
    case 'w':
    	posh -= 10;
      hm1.Set_Steer_position_runtime(posh, t);
    	break;
    case 's':
	    posh += 10;

	    hm1.Set_Steer_position_runtime(posh, t);
    	break;
    case 'a':
    	posr += 10;
      rotate.Set_Steer_position_runtime(posr, t);
    	break;
    case 'x':
    	posr -= 10;
      rotate.Set_Steer_position_runtime(posr, t);
    	break;


    case 'A':
    	pose += 10;
      em2.Set_Steer_position_runtime(pose, t);
    	break;
    case 'X':
	    pose -= 10;
	    em2.Set_Steer_position_runtime(pose, t);
    	break;
    case 'W':
    	posg -= 10;
      grip.Set_Steer_position_runtime(1000, t);
    	break;
    case 'S':
    	posg += 10;
      grip.Set_Steer_position_runtime(3000, t);
    	break;

    default:
      break;
  }
  
}



