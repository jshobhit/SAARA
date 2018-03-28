import processing.serial.*;
import g4p_controls.*;
// The serial port:
Serial x;       


color fillVal = color(0);
Boolean button = true;

void setup() {
  size(1200, 800);
  noStroke();
  background(#F2F2F2);
  createGUI();
  //printArray(Serial.list()); // do this to find your arduino
  // Open the port you are using at the rate you want:
  x = new Serial(this, Serial.list()[0], 115200);
  x.clear();
}
String input = "";

void draw() { 

}

//The keyPressed() function is called once every time a key is pressed.
// see https://processing.org/reference/keyCode.html

void keyPressed() {
  if (key == CODED) {
    switch(keyCode) {
    case UP: 
      x.write('u');
      fillVal = #FF0000;
      break;
    case DOWN: 
      x.write('d');
      fillVal = #00FF00;
      break;    
    case LEFT: 
      x.write('l');
      fillVal = #0000FF;
      break;    
    case RIGHT: 
      x.write('r');
      fillVal = #00FFF0;
      break;
    case CONTROL:
      x.write('m');
      if(button == true){
         label1.setText("Current Mode : 2");
         button = false;
      }
      else{
        label1.setText("Current Mode : 1");
        button = true;
      }
      break;
     default:
      //fillVal = #000000;
    }
  } 
  else if(keyPressed){
      switch(key){
        case 'w':
          x.write('w');
          break;
        case 's':
          x.write('s');
          break;
        case 'a':
          x.write('a');
          break;
        case 'd':
          x.write('x');
          break;
        case 'W':
          x.write('W');
          break;
        case 'S':
          x.write('S');
          break;
        case 'A':
          x.write('A');
          break;
        case 'D':
          x.write('X');
          break;
        default:
          break; 
      }
    }
    
}


void serialEvent(Serial p) {
  switch(p.read()) {
  case 'R': 
    fillVal = #FF0000;
    break;
  case 'G': 
    fillVal = #00FF00;
    break;    
  case 'Y': 
    fillVal = #FFFF00;
    break;    
  case 'O': 
    fillVal = #FF7F00;
    break;
  case 'q':
    //label1.setText("     ");
    //label1.setText("Current Mode : 1");
    break;
  case 'Q':
    //label1.setText("     ");
    //label1.setText("Current Mode : 2");
    break;
  case 'x':
    
 //  printToLabel(label_x, 43);
   break;
  case 'y':
    break;
  case 'z':
    break;
  case 'a':
    break;
  case 'b':
    break;
  case 'c':
    break;
  default: 
    fillVal = #000000;
    break;
  }
} 