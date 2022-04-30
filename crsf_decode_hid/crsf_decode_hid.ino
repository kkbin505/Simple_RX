#include "CRSF.h"
#include <Streaming.h>
//https://github.com/janelia-arduino/Streaming
#include <Joystick.h>
//https://github.com/MHeironimus/ArduinoJoystickLibrary



// twelve servo objects can be created on most boards
Joystick_ Joystick;
const bool testAutoSendMode = false;
  

int moveTime = 4000; // Update every 4ms
unsigned long int currentMicros = 0;

CRSF crsf;


void setup(){
  crsf.begin();
 // Serial.begin(115200);
  Joystick.setXAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setYAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setZAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRxAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  //Joystick.setThrottleRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
 // Joystick.setRudderRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
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
  //crsf.FeedLine();


  if (micros() - currentMicros >= moveTime) {
    crsf.GetCrsfPacket();
    if(crsf.crsfData[1]==24){
      // Turn indicator light on.
      digitalWrite(LED_BUILTIN, 1);
      crsf.UpdateChannels();
     // servoPos=map(crsf.channels[1],CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX,1000,2000);
    Joystick.setXAxis(crsf.channels[0]);
    Joystick.setYAxis(crsf.channels[1]);
    Joystick.setZAxis(crsf.channels[2]);
    Joystick.setRxAxis(crsf.channels[3]);
  

  }
  else{
     digitalWrite(LED_BUILTIN, 0);
  }
    if (testAutoSendMode == false)
    {
 
    }
    currentMicros = micros();
    Joystick.sendState();
    //delay(5);
  }



    //Serial.print(crsf.channels[0]); 
    //Serial.print(crsf.channels[1]); 
    //Serial.print(crsf.channels[2]); 
    //Serial.println();
    //Serial<<crsf.channels[0]<<","<<crsf.channels[1]<<","<<crsf.channels[2]<<","<<crsf.channels[3]<<"\r\n";
   // Serial<<crsf.crsfData[0]<<","<<crsf.crsfData[1]<<","<<crsf.crsfData[2]<<","<<crsf.crsfData[3]<<"\r\n";
   // Serial<<crsf.channels[0]<<","<<crsf.channels[1]<<","<<crsf.channels[2]<<","<<crsf.channels[3]<<","<<crsf.inBuffer[4]<<","<<crsf.inBuffer[5]<<","<<crsf.inBuffer[24]<<","<<crsf.inBuffer[25]<<"\r\n";
    //Serial.println(startMillis);
//    Serial.println(crsf.GetBufferIndex());
}
