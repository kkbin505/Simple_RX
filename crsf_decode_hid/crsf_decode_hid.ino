/*
  Simple Arduino trasmisster
  https://github.com/kkbin505/Simple_RX

 * This library is developed to decode crsf protocol for AVR 8bit micro controller
 * This file is part of Simple RX
 *
 * Simple RX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Simple RX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

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
  //Serial.begin(115200);
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
    //Serial<<crsf.inBuffer[0]<<","<<crsf.inBuffer[1]<<","<<crsf.inBuffer[2]<<","<<crsf.inBuffer[3]<<","<<crsf.inBuffer[4]<<","<<crsf.inBuffer[5]<<","<<crsf.inBuffer[6]<<","<<crsf.inBuffer[18]<<"\r\n";
    //Serial.println(startMillis);
//    Serial.println(crsf.GetBufferIndex());
}
