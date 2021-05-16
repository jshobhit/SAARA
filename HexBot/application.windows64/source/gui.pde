/* =========================================================
 * ====                   WARNING                        ===
 * =========================================================
 * The code in this tab has been generated from the GUI form
 * designer and care should be taken when editing this file.
 * Only add/edit code inside the event handlers i.e. only
 * use lines between the matching comment tags. e.g.

 void myBtnEvents(GButton button) { //_CODE_:button1:12356:
     // It is safe to enter your event code here  
 } //_CODE_:button1:12356:
 
 * Do not rename this tab!
 * =========================================================
 */

public void imgButton1_click1(GImageButton source, GEvent event) { //_CODE_:imgButton1:252065:
   println("imgButton1 - GImageButton >> GEvent." + event + " @ " + millis());
} //_CODE_:imgButton1:252065:

public void torquebut_click1(GButton source, GEvent event) { //_CODE_:torquebut:685831:
  x.write('t');
  //x.write(t);
  println("button1 - GButton >> GEvent." + event + " @ " + millis());
} //_CODE_:torquebut:685831:

public void imgButton2_click1(GImageButton source, GEvent event) { //_CODE_:imgButton2:208253:
  println("imgButton2 - GImageButton >> GEvent." + event + " @ " + millis());
} //_CODE_:imgButton2:208253:

public void pumpbut_click1(GButton source, GEvent event) { //_CODE_:pumpbut:759585:
  x.write('p');
  println("pumpbut - GButton >> GEvent." + event + " @ " + millis());
} //_CODE_:pumpbut:759585:



// Create all the GUI controls. 
// autogenerated do not edit
public void createGUI(){
  G4P.messagesEnabled(false);
  G4P.setGlobalColorScheme(GCScheme.BLUE_SCHEME);
  G4P.setCursor(ARROW);
  surface.setTitle("Sketch Window");
  imgButton1 = new GImageButton(this, 5, -4, new String[] { "Picture1.png", "Picture1.png", "Picture1.png" } );
  imgButton1.addEventHandler(this, "imgButton1_click1");
  torquebut = new GButton(this, 17, 376, 126, 28);
  torquebut.setText("Torque On/Off");
  torquebut.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  torquebut.addEventHandler(this, "torquebut_click1");
  imgButton2 = new GImageButton(this, 385, 355, new String[] { "Picture2.png", "Picture2.png", "Picture2.png" } );
  imgButton2.addEventHandler(this, "imgButton2_click1");
  label1 = new GLabel(this, 255, 376, 120, 30);
  label1.setTextAlign(GAlign.CENTER, GAlign.MIDDLE);
  label1.setText("Current Mode : 1");
  label1.setOpaque(true);
  pumpbut = new GButton(this, 147, 376, 105, 26);
  pumpbut.setText("Pump On/Off");
  pumpbut.setLocalColorScheme(GCScheme.CYAN_SCHEME);
  pumpbut.addEventHandler(this, "pumpbut_click1");
}

// Variable declarations 
// autogenerated do not edit
GImageButton imgButton1; 
GButton torquebut; 
GImageButton imgButton2; 
GLabel label1; 
GButton pumpbut; 