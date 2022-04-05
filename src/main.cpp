#include <Arduino.h>
#include <CRSF.h>
#include <Streaming.h>


CRSF crsf;


void setup() {
  // put your setup code here, to run once:
  crsf.begin();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  //crsf.FeedLine();
  crsf.GetCrsfPacket();
  if (crsf.toChannels == 1){
    //crsf.UpdateServos();

    crsf.toChannels = 0;


    
  }
    
  crsf.UpdateChannels();
    Serial<<crsf.channels[0]<<","<<crsf.channels[1]<<","<<crsf.channels[2]<<","<<crsf.channels[3]<<"\r\n";
    //Serial<<crsf.crsfData[0]<<","<<crsf.crsfData[1]<<","<<crsf.crsfData[3]<<"\r\n";
//    Serial.println(crsf.GetBufferIndex());
}