#include "CRSF.h"

void CRSF::begin(){
	uint8_t loc_crsfData[CRSF_PACKET_SIZE] = {
	  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	int16_t loc_channels[16]  = {
	  		1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023};
	int16_t loc_servos[16]    = {
  			1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023};
  	port.begin(SERIAL_BAUDRATE);

  //initial data;
	memcpy(crsfData,loc_crsfData,CRSF_PACKET_SIZE);
	memcpy(channels,loc_channels,CRSF_MAX_CHANNEL);
	failsafe_status = CRSF_SIGNAL_OK;
	crsf_passthrough = 1;
	toChannels = 0;
	bufferIndex = 0;
	feedState = 0;
  isDataFrame = 0;

}

int16_t CRSF::Channel(uint8_t ch) {
  // Read channel data
  if ((ch>=0)&&(ch<16)){
    return channels[ch];
  }
  else{
    return CRSF_CHANNEL_MID;
  }
}
/*
uint8_t CRSF::DigiChannel(uint8_t ch) {
  // Read digital channel data
  if ((ch>0) && (ch<=2)) {
    return channels[15+ch];
  }
  else{
    return 0;
  }
}
void CRSF::Servo(uint8_t ch, int16_t position) {
  // Set servo position
  if ((ch>0)&&(ch<=16)) {
    if (position>2048) {
      position=2048;
    }
    servos[ch-1] = position;
  }
}
void CRSF::DigiServo(uint8_t ch, uint8_t position) {
  // Set digital servo position
  if ((ch>0) && (ch<=2)) {
    if (position>1) {
      position=1;
    }
    servos[15+ch] = position;
  }
}
*/
uint8_t CRSF::Failsafe(void) {
  return failsafe_status;
}

void CRSF::PassthroughSet(int mode) {
  // Set passtrough mode, if true, received channel data is send to servos
  crsf_passthrough = mode;
}

int CRSF::PassthroughRet(void) {
  // Return current passthrough mode
  return crsf_passthrough;
}
void CRSF::UpdateServos(void) {
  // Send data to servos
  // Passtrough mode = false >> send own servo data
  // Passtrough mode = true >> send received channel data
  uint8_t i;
  if (crsf_passthrough==0) {
    // clear received channel data
    for (i=1; i<24; i++) {
      crsfData[i] = 0;
    }

    // reset counters
    ch = 0;
    bit_in_servo = 0;
    byte_in_crsf = 1;
    bit_in_crsf = 0;

    // store servo data
    for (i=0; i<176; i++) {
      if (servos[ch] & (1<<bit_in_servo)) {
        crsfData[byte_in_crsf] |= (1<<bit_in_crsf);
      }
      bit_in_crsf++;
      bit_in_servo++;

      if (bit_in_crsf == 8) {
        bit_in_crsf =0;
        byte_in_crsf++;
      }
      if (bit_in_servo == 11) {
        bit_in_servo =0;
        ch++;
      }
    }


    // Failsafe
    if (failsafe_status == CRSF_SIGNAL_LOST) {
      crsfData[23] |= (1<<2);
    }

    if (failsafe_status == CRSF_SIGNAL_FAILSAFE) {
      crsfData[23] |= (1<<2);
      crsfData[23] |= (1<<3);
    }
  }
  // send data out
  //serialPort.write(crsfData,25);
  for (i=0;i<25;i++) {
    port.write(crsfData[i]);
  }
}
void CRSF::UpdateChannels(void) {
  //uint8_t i;
  //uint8_t crsf_pointer = 0;
  // clear channels[]
  /*for (i=0; i<16; i++) {
    channels[i] = 0;
  }

  // reset counters
  byte_in_crsf = 1;
  bit_in_crsf = 0;
  ch = 0;
  bit_in_channel = 0;
  //this method is much slower than the other method
  // process actual crsf data
  for (i=0; i<176; i++) {
    if (crsfData[byte_in_crsf] & (1<<bit_in_crsf)) {
      channels[ch] |= (1<<bit_in_channel);
    }
    bit_in_crsf++;
    bit_in_channel++;

    if (bit_in_crsf == 8) {
      bit_in_crsf =0;
      byte_in_crsf++;
    }
    if (bit_in_channel == 11) {
      bit_in_channel =0;
      ch++;
    }
  }*/
  //if(channels[25]==crsf_crc8(&channels[2], CRSF_PACKET_SIZE-3)){
  //  failsafe_status = CRSF_SIGNAL_OK;

    channels[0]  = ((crsfData[3]|crsfData[4]<< 8) & 0x07FF);
    channels[1]  = ((crsfData[4]>>3|crsfData[5]<<5) & 0x07FF);
    channels[2]  = ((crsfData[5]>>6|crsfData[6]<<2|crsfData[7]<<10) & 0x07FF);
    channels[3]  = ((crsfData[7]>>1|crsfData[8]<<7) & 0x07FF);
    channels[4]  = ((crsfData[8]>>4|crsfData[9]<<4) & 0x07FF);
    channels[5]  = ((crsfData[9]>>7|crsfData[10]<<1|crsfData[11]<<9) & 0x07FF);
    channels[6]  = ((crsfData[11]>>2|crsfData[12]<<6) & 0x07FF);
    channels[7]  = ((crsfData[12]>>5|crsfData[13]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
    channels[8]  = ((crsfData[14]|crsfData[15]<< 8) & 0x07FF);
    channels[9]  = ((crsfData[15]>>3|crsfData[16]<<5) & 0x07FF);
    channels[10] = ((crsfData[16]>>6|crsfData[17]<<2|crsfData[18]<<10) & 0x07FF);
    channels[11] = ((crsfData[18]>>1|crsfData[19]<<7) & 0x07FF);
    channels[12] = ((crsfData[19]>>4|crsfData[20]<<4) & 0x07FF);
    channels[13] = ((crsfData[20]>>7|crsfData[21]<<1|crsfData[22]<<9) & 0x07FF);
    channels[14] = ((crsfData[22]>>2|crsfData[23]<<6) & 0x07FF);
    channels[15] = ((crsfData[23]>>5|crsfData[24]<<3) & 0x07FF);
  //}

}

void CRSF::FeedLine(void){
  if (port.available() > 26){
    while(port.available() > 0){
      inData = port.read();
      //currentMicros = micros();
      switch (feedState){
      case 0:
        if (inData != 0xC8){
          while(port.available() > 0){//read the contents of in buffer this should resync the transmission
            inData = port.read();
          }
          return;
        }
        else{
          bufferIndex = 0;
          inBuffer[bufferIndex] = inData;
          //inBuffer[26] = 0xff;
          feedState = 1;
        }
        break;
      case 1:
        bufferIndex ++;
        inBuffer[bufferIndex] = inData;
        if (bufferIndex < 26 && port.available() == 0){
          feedState = 0;
        }
        if (bufferIndex == 26){
          feedState = 0;
          if (inBuffer[0]==0xC8){
            memcpy(crsfData,inBuffer,26);
            toChannels = 1;
          }
        }
        break;
      }
    }
  }
}

//Crc8 _crc;
		static uint8_t crsf_crc8tab[256] = {
    	0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    	0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    	0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    	0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    	0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    	0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
   		0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    	0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    	0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    	0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    	0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    	0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    	0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    	0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    	0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    	0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9};

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len) {
    u8 crc = 0;
    for (u8 i=0; i < len; i++) {
        crc = crsf_crc8tab[crc ^ *ptr++];
    }
    return crc;
}

void CRSF::GetCrsfPacket(void){
    uint8_t crc;
    bufferIndex=0;
    while(port.available() > 0){
      inData = port.read();
      if (bufferIndex==0){
        if(inData == CRSF_ADDRESS_FLIGHT_CONTROLLER){
          inBuffer[bufferIndex++] = inData;
          inData = port.read();
          frameLength = inData;
          inBuffer[bufferIndex++] = inData;
        }else{
          bufferIndex=0;
        }
      }else if(bufferIndex >1 && bufferIndex < frameLength + 1){
          inBuffer[bufferIndex++] = inData;
      }else if(bufferIndex == frameLength + 1){
        //calculate received packet crc
        uint8_t inCrc=inBuffer[CRSF_FRAME_LENGTH-1];
        uint8_t crc=crsf_crc8(&inBuffer[2],inBuffer[1]-1);
        //If crc is correct copy buffer data to crsfData
        if( frameLength==CRSF_FRAME_LENGTH && inBuffer[0]== CRSF_ADDRESS_FLIGHT_CONTROLLER){
          memcpy(crsfData,inBuffer,CRSF_PACKET_SIZE);
        }
        bufferIndex = 0;
      }
         // inBuffer[25]=port.available();

    }

     /* if(bufferIndex==0 && inData!=CRSF_ADDRESS_FLIGHT_CONTROLLER)
        continue;
      inBuffer[bufferIndex++] = inData;
      if (inBuffer[1]==0x18){

   

      if(bufferIndex==CRSF_PACKET_SIZE-1){
        uint8_t inCrc=inBuffer[CRSF_PACKET_SIZE-1];
        uint8_t crc=crsf_crc8(&inBuffer[2],CRSF_PACKET_SIZE-3);

        //if( inBuffer[0]==CRSF_ADDRESS_FLIGHT_CONTROLLER  ){
          memcpy(crsfData,inBuffer,26);
        //}
      bufferIndex=0 ;
         }
         else{
           bufferIndex=0 ;
         }

      }*/
   
}


void CRSF::CheckPacketTimeout(){
  if ( micros() - currentMicros > CRSF_PACKET_TIMEOUT_US){
    failsafe_status = CRSF_SIGNAL_LOST;
  }else{
    failsafe_status = CRSF_SIGNAL_OK;
  }
  return failsafe_status;
}





