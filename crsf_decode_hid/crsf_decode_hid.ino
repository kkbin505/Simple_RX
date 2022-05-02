#include "CRSF.h"
#include <Streaming.h>
//https://github.com/janelia-arduino/Streaming
#include <Joystick.h>
//https://github.com/MHeironimus/ArduinoJoystickLibrary

//#define DEBUG // if not commented out, Serial.print() is active! For debugging only!!

// twelve servo objects can be created on most boards
//Joystick_ Joystick;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  0, 0,                 // Button Count, Hat Switch Count
  true, true, true,  // X, Y, Z
  true, true, true,  // Rx, Ry, Rz
  false, false,          // Rudder, Throttle
  false, false, false);    // Accelerator, Brake, Steering
  
const bool testAutoSendMode = false;
#define FRESH_TIME_US 4000  // Update every 4ms
unsigned long int currentMicros = 0;

int packetPerSec;
unsigned long int currentMillis;

#define SECOND 1000


CRSF crsf;

void setup(){
  crsf.begin();

  #ifdef DEBUG
    Serial.begin(115200);
    packetPerSec = 0;
    currentMillis = 0;
  #endif

  Joystick.setXAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setYAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setZAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRxAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRyAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRzAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  //delay(5);
  
  if (micros() - currentMicros >= FRESH_TIME_US) {
    crsf.GetCrsfPacket();
    //if(crsf.crsfData[1]==24){ //data packet
      #ifdef DEBUG
        packetPerSec++;
      #endif
      // Turn indicator light on.
      if(crsf.failsafe_status == 0){
        digitalWrite(LED_BUILTIN, 1);
      }else{
        digitalWrite(LED_BUILTIN, 0);
      }
      crsf.UpdateChannels();
      Joystick.setXAxis(crsf.channels[0]);
      Joystick.setYAxis(crsf.channels[1]);
      Joystick.setZAxis(crsf.channels[2]);
      Joystick.setRxAxis(crsf.channels[3]);
      Joystick.setRyAxis(crsf.channels[4]);
      Joystick.setRzAxis(crsf.channels[5]);
 // }


    currentMicros = micros();
    Joystick.sendState();
  }


  #ifdef DEBUG
    if(millis()-currentMillis > SECOND ){
      Serial.println(packetPerSec); 
      currentMillis=millis();
      packetPerSec=0;
    }
    //Serial.print(crsf.channels[0]); 
    //Serial.print(crsf.channels[0]); 
    //Serial.print(crsf.channels[1]); 
    //Serial.print(crsf.channels[2]); 
    //Serial.println();
    Serial<<crsf.channels[0]<<","<<crsf.channels[1]<<","<<crsf.channels[2]<<","<<crsf.channels[3]<<"\r\n";
   // Serial<<crsf.crsfData[0]<<","<<crsf.crsfData[1]<<","<<crsf.crsfData[2]<<","<<crsf.crsfData[3]<<"\r\n";
   // Serial<<crsf.channels[0]<<","<<crsf.channels[1]<<","<<crsf.channels[2]<<","<<crsf.channels[3]<<","<<crsf.inBuffer[4]<<","<<crsf.inBuffer[5]<<","<<crsf.inBuffer[24]<<","<<crsf.inBuffer[25]<<"\r\n";
    //Serial.println(startMillis);
    //Serial.println(crsf.GetBufferIndex());
  #endif

  
}
