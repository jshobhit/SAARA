import processing.serial.*;
import g4p_controls.*;
// The serial port:
Serial x;       


color fillVal = color(0);

void setup() {
  size(800, 460);
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
      break;
    case DOWN: 
      x.write('d');
      break;    
    case LEFT: 
      x.write('l');
      break;    
    case RIGHT: 
      x.write('r');
      break;
    case CONTROL:
      x.write('m');
      break;
     default:
     break;
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
  case 'q':
    label1.setText("     ");
    label1.setText("Current Mode : 1");
    break;
  case 'Q':
    label1.setText("     ");
    label1.setText("Current Mode : 2");
    break;
  default: 
    break;
  }
} 