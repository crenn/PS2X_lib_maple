#include "PS2X_lib.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
//#include <avr/io.h>
#include "WProgram.h"
//#include "pins_arduino.h"



static byte enter_config[]={0x01,0x43,0x00,0x01,0x00};
static byte set_mode[]={0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
static byte set_bytes_large[]={0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};
static byte exit_config[]={0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
static byte enable_rumble[]={0x01,0x4D,0x00,0x00,0x01};

boolean PS2X::NewButtonState() {
   return ((last_buttons ^ buttons) > 0);

}

boolean PS2X::NewButtonState(unsigned int button) {
  return (((last_buttons ^ buttons) & button) > 0);
}

boolean PS2X::ButtonPressed(unsigned int button) {
  return(NewButtonState(button) & Button(button));
}

boolean PS2X::ButtonReleased(unsigned int button) {
  return((NewButtonState(button)) & ((~last_buttons & button) > 0));
}
  
boolean PS2X::Button(uint16_t button) {
   return ((~buttons & button) > 0);
}

unsigned int PS2X::ButtonDataByte() {
   return (~buttons);
}

byte PS2X::Analog(byte button) {
  return PS2data[button];
}
unsigned char PS2X::_gamepad_shiftinout (char byte) {
   unsigned char tmp = SPIn.transfer(byte);
   delayMicroseconds(CTRL_BYTE_DELAY); // Waiting for ack signal which isn't connected
   return tmp;
}

void PS2X::read_gamepad(boolean motor1, byte motor2) {
  double temp = millis() - last_read;
  
  if (temp > 1500) //waited to long
    reconfig_gamepad();
    
  if(temp < read_delay)  //waited too short
    delay(read_delay - temp);
    
    
  last_buttons = buttons; //store the previous buttons states

if(motor2 != 0x00)
  motor2 = map(motor2,0,255,0x40,0xFF); //noting below 40 will make it spin
    
    digitalWrite(CSpin, LOW); //CLR(*_att_oreg,_att_mask); // low enable joystick
    delayMicroseconds(CTRL_BYTE_DELAY);
    //Send the command to send button and joystick data;
    char dword[9] = {0x01,0x42,0,motor1,motor2,0,0,0,0};
    byte dword2[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	#ifdef PS2X_COM_DEBUG
    SerialUSB.println("OUT:IN");
	#endif
    for (int i = 0; i<9; i++) {
	  PS2data[i] = _gamepad_shiftinout(dword[i]);
	  #ifdef PS2X_COM_DEBUG
		SerialUSB.print(dword[i], HEX);
		SerialUSB.print(":");
		SerialUSB.println(PS2data[i], HEX);
	  #endif
    }
    if(PS2data[1] == 0x79) {  //if controller is in full data return mode, get the rest of data
       for (int i = 0; i<12; i++) {
	  PS2data[i+9] = _gamepad_shiftinout(dword2[i]);
	  #ifdef PS2X_COM_DEBUG
		SerialUSB.print(dword[i], HEX);
		SerialUSB.print(":");
		SerialUSB.println(PS2data[i], HEX);
	  #endif
       }
    }
    
    digitalWrite(CSpin, HIGH); //SET(*_att_oreg,_att_mask); // HI disable joystick
    
   buttons = *(uint16_t*)(PS2data+3);   //store as one value for multiple functions
   last_read = millis();
}

byte PS2X::config_gamepad(uint8_t att) {
  
  SPIn.end();
  SPIn.begin(SPIspeed, LSBFIRST, 3);
  
  pinMode(SPIn.misoPin(), INPUT_PULLUP);
  
  CSpin = att;
  pinMode(CSpin, OUTPUT);
    
   
   //new error checking. First, read gamepad a few times to see if it's talking
   read_gamepad();
   read_gamepad();
   
   //see if it talked
   if(PS2data[1] != 0x41 && PS2data[1] != 0x73 && PS2data[1] != 0x79){ //see if mode came back. If still anything but 41, 73 or 79, then it's not talking
      #ifdef PS2X_DEBUG
		SerialUSB.println("Controller mode not matched or no controller found");
		SerialUSB.print("Expected 0x41 or 0x73, got ");
		SerialUSB.println(PS2data[1], HEX);
	  #endif
	 
	 return 1; //return error code 1
	}
  
  //try setting mode, increasing delays if need be. 
  
  for(int y = 0; y <= 10; y++)
  {
   sendCommandString(enter_config, sizeof(enter_config));
   sendCommandString(set_mode, sizeof(set_mode));
   sendCommandString(exit_config, sizeof(exit_config));
   
   read_gamepad();
   
    if(PS2data[1] == 0x73)
      break;
      
    if(y == 10){
		#ifdef PS2X_DEBUG
		SerialUSB.println("Controller not accepting commands");
		SerialUSB.print("mode stil set at");
		SerialUSB.println(PS2data[1], HEX);
		#endif
      return 2; //exit function with error
	  }
    
    read_delay += 10; //add 10ms to read_delay
  }
   
 return 0; //no error if here
}

void PS2X::sendCommandString(byte string[], byte len) {
  
   digitalWrite (CSpin, LOW); //CLR(*_att_oreg,_att_mask); // low enable joystick
  for (int y=0; y < len; y++)
    _gamepad_shiftinout(string[y]);
    
   digitalWrite (CSpin, HIGH); //SET(*_att_oreg,_att_mask); //high disable joystick  
   delay(read_delay);                  //wait a few
}

void PS2X::enableRumble() {
  
     sendCommandString(enter_config, sizeof(enter_config));
     sendCommandString(enable_rumble, sizeof(enable_rumble));
     sendCommandString(exit_config, sizeof(exit_config));
     en_Rumble = true;
  
}

void PS2X::enablePressures() {
  
     sendCommandString(enter_config, sizeof(enter_config));
     sendCommandString(set_bytes_large, sizeof(set_bytes_large));
     sendCommandString(exit_config, sizeof(exit_config));
     en_Pressures = true;
}

void PS2X::reconfig_gamepad(){
  
   sendCommandString(enter_config, sizeof(enter_config));
   sendCommandString(set_mode, sizeof(set_mode));
   if (en_Rumble)
      sendCommandString(enable_rumble, sizeof(enable_rumble));
   if (en_Pressures)
      sendCommandString(set_bytes_large, sizeof(set_bytes_large));
   sendCommandString(exit_config, sizeof(exit_config));
   
}
