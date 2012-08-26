#include <PS2X_lib.h>

HardwareSPI spi(2);
PS2X ps2x(spi, SPI_562_500KHZ); // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte vibrate = 0;

void setup(){
  delay(5000);
  
 error = ps2x.config_gamepad(31);   //setup GamePad(clock, command, attention, data) pins, check for error
 
 if(error == 0){
   SerialUSB.println("Found Controller, configured successful");
   SerialUSB.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  SerialUSB.println("holding L1 or R1 will print out the analog stick values.");
  SerialUSB.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1)
   SerialUSB.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   SerialUSB.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
   //SerialUSB.print(ps2x.Analog(1), HEX);
 
 
 ps2x.enableRumble();              //enable rumble vibration motors
 ps2x.enablePressures();           //enable reading the pressure values from the buttons. 
  

  
}

void loop(){
   /* You must Read Gamepad to get new values
   Read GamePad and set vibration values
   ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
   if you don't enable the rumble, use ps2x.read_gamepad(); with no values
   
   you should call this at least once a second
   */
 if(error != 0) {
  SerialUSB.println("Error during startup");
  delay(1000);
  return; 
 }
  
  ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
  
  if(ps2x.Button(PSB_START))                   //will be TRUE as long as button is pressed
       SerialUSB.println("Start is being held");
  if(ps2x.Button(PSB_SELECT))
       SerialUSB.println("Select is being held");
       
       
   if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
     SerialUSB.print("Up held this hard: ");
     SerialUSB.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
     SerialUSB.print("Right held this hard: ");
      SerialUSB.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
     SerialUSB.print("LEFT held this hard: ");
      SerialUSB.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
     SerialUSB.print("DOWN held this hard: ");
   SerialUSB.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }   

  
    vibrate = ps2x.Analog(PSAB_BLUE);        //this will set the large motor vibrate speed based on 
                                            //how hard you press the blue (X) button    
  
  if (ps2x.NewButtonState())               //will be TRUE if any button changes state (on to off, or off to on)
  {
      if(ps2x.Button(PSB_L3))
       SerialUSB.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
       SerialUSB.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
       SerialUSB.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
       SerialUSB.println("R2 pressed");
      if(ps2x.Button(PSB_GREEN))
       SerialUSB.println("Triangle pressed");
       
  }   
       
  
  if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
       SerialUSB.println("Circle just pressed");
       
  if(ps2x.ButtonReleased(PSB_PINK))             //will be TRUE if button was JUST released
       SerialUSB.println("Square just released");     
  
  if(ps2x.NewButtonState(PSB_BLUE))            //will be TRUE if button was JUST pressed OR released
       SerialUSB.println("X just changed");    
  
  
  if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) // print stick values if either is TRUE
  {
      SerialUSB.print("Stick Values:");
      SerialUSB.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      SerialUSB.print(",");
      SerialUSB.print(ps2x.Analog(PSS_LX), DEC); 
      SerialUSB.print(",");
      SerialUSB.print(ps2x.Analog(PSS_RY), DEC); 
      SerialUSB.print(",");
      SerialUSB.println(ps2x.Analog(PSS_RX), DEC); 
  } 
    
 delay(100);
}