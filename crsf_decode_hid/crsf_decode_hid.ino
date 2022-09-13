/*
  This an Arduino Micro based USBHID interface for RC receivers
  Could be used for simulator
  Feature: fast fresh rate
  Support:
  1. Expresslrs (crsf @ 115200 baud rate)
  2. PWM (333Hz max)
  3. PPM 
  
*/
#include "AVRPort23.h"
#include "CRSF.h"
#include <Streaming.h>
//https://github.com/janelia-arduino/Streaming
#include <Joystick.h>
//https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <PPMReader.h>
//https://github.com/DzikuVx/PPMReader

//********************************Define Line *****************************************
/* Define working type uncomment 1 at once
  1.crsf to usb hid  (elrs receiver flash to 115200 baud rate)
  2.pwm to usb hid
  3.ppm to usb hid
  4.debug mode
  */
//#define CRSF_TO_USBHID
//#define PWM_TO_USBHID
#define PPM_TO_USBHID
#define DEBUG // if not commented out, Serial.print() is active! For debugging only!!

//define pwm channel range
#define PWM_CHANNEL_MIN 1000
#define PWM_CHANNEL_MID 1500
#define PWM_CHANNEL_MAX 2000

//Joystick_ Joystick;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  0, 0,                 // Button Count, Hat Switch Count
  true, true, false,  // X, Y, Z
  true, true, false,  // Rx, Ry, Rz
  false, false,          // Rudder, Throttle
  false, false, false);    // Accelerator, Brake, Steering
  
const bool testAutoSendMode = false;
#define FRESH_TIME_US 3000  // Update  joystick data every 1ms give enougth time for crsf frame decode
unsigned long int currentMicros = 0;

// channel data max 8 channel for crsf and ppm
int channelData[8];
//We create variables for the time width values of each PWM input signal (only support 4 channel, if more channel was needed, highly recommand use ppm or crsf receiver)
unsigned long counter_1, counter_2, counter_3, counter_4, current_count;

//We create 4 variables to stopre the previous value of the input signal (if LOW or HIGH)
byte last_CH1_state, last_CH2_state, last_CH3_state, last_CH4_state;


int packetPerSec;
unsigned long int currentMillis;

#define SECOND 1000


uint32_t last_interrupt_time = 0;
uint32_t timer_start = 0;
uint8_t pulse_time = 0;

#ifdef CRSF_TO_USBHID
  CRSF crsf;
#endif

#ifdef PWM_TO_USBHID
  //PWM pin define arduino pro micro
  // D0 - TX
  // D1 - RX
  // D2 - D2
  // D3 - D3
  // 5V - 5V
  // GND - GND
  #define CHAN1 D,0
  #define _INT1 0
  #define CHAN2 D,1
  #define _INT2 1
  #define CHAN3 D,3
  #define _INT3 3
  #define CHAN4 D,2
  #define _INT4 2
  volatile unsigned long Time[4];
  volatile unsigned int Value[4];
  volatile bool ValChanged[4];
  unsigned int NewValue[4];

#endif


#ifdef PPM_TO_USBHID
  //PPM pin define arduino pro micro
  // D3 - sinal
  // GND - GND
  PPMReader ppmReader(3, 0, false);
#endif

#ifdef DEBUG
  unsigned long int serialPrintMillis;
#endif 
/*
void calcSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(pwmChannelPin1) == HIGH) 
    { 
        timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start != 0)
        { 
            //record the pulse time
            pulse_time = ((volatile int)micros() - timer_start);
            //restart the timer
            timer_start = 0;
        }
    } 
} 
*/

void setup(){

  currentMillis = 0;
  
  #ifdef CRSF_TO_USBHID
    crsf.begin();
  #endif

  #ifdef PWM_TO_USBHID
    //PCICR |= (1 << PCIE0);    //enable PCMSK0 scan                                                 
    //PCMSK0 |= (1 << PCINT4);  //Set pin D8 trigger an interrupt on state change.
    //PCMSK0 |= (1 << PCINT5);  //Set pin D9 trigger an interrupt on state change.
    portMode(CHAN1, INPUT, HIGH);
    portMode(CHAN2, INPUT, HIGH);
    portMode(CHAN3, INPUT, HIGH);
    portMode(CHAN4, INPUT, HIGH);

    attachInterrupt(_INT1, isr1, CHANGE);
    attachInterrupt(_INT2, isr2, CHANGE);
    attachInterrupt(_INT3, isr3, CHANGE);
    attachInterrupt(_INT4, isr4, CHANGE);

  #endif

  //#ifdef PPM_TO_USBHID
    //no need to add code here
  //#endif

  #ifdef DEBUG
    Serial.begin(115200);
    packetPerSec = 0;
    serialPrintMillis = 0;
  #endif

  //inital channel data
  for (int i=0; i<8; i++) {
      channelData[i]=PWM_CHANNEL_MID;
  }


  Joystick.setXAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setYAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  //Joystick.setZAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRxAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  Joystick.setRyAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
  //Joystick.setRzAxisRange(CRSF_CHANNEL_MIN,CRSF_CHANNEL_MAX);
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
  
  if (micros() - currentMicros >= FRESH_TIME_US) {
    #ifdef CRSF_TO_USBHID
    crsf.GetCrsfPacket();
    //if(crsf.crsfData[1]==24){ //data packet
     // #ifdef DEBUG
      //  packetPerSec++;
      //#endif
      // Turn indicator light on.
      if(crsf.failsafe_status == 0){
        digitalWrite(LED_BUILTIN, 1);
      }else{
        digitalWrite(LED_BUILTIN, 0);
      }
      crsf.UpdateChannels();
      channelData[0] = crsf.channels[0];
      channelData[1] = crsf.channels[1];
      channelData[2] = crsf.channels[2];
      channelData[3] = crsf.channels[3];
      //channelData[4] = crsf.channels[4];
      //channelData[5] = crsf.channels[5];
    #endif

    #ifdef PWM_TO_USBHID
      if (ValChanged[0])
        {
          NewValue[0] = (NewValue[0]+Value[0])/2;
          channelData[0] = map(NewValue[0],PWM_CHANNEL_MIN, PWM_CHANNEL_MAX, CRSF_CHANNEL_MIN, CRSF_CHANNEL_MAX);
          ValChanged[0] = false;
        }

        if (ValChanged[1])
        {
          NewValue[1] = (NewValue[1]+Value[1])/2;
          channelData[1] = map(NewValue[1],PWM_CHANNEL_MIN, PWM_CHANNEL_MAX, CRSF_CHANNEL_MIN, CRSF_CHANNEL_MAX);
          ValChanged[1] = false;
        }
        
        if (ValChanged[2])
        {
          NewValue[2] = (NewValue[2]+Value[2])/2;
          channelData[2] = map(NewValue[2],PWM_CHANNEL_MIN, PWM_CHANNEL_MAX, CRSF_CHANNEL_MIN, CRSF_CHANNEL_MAX);
          //channelData[2] = CRSF_CHANNEL_MID;
          ValChanged[2] = false;
        }
        
        if (ValChanged[3])
        {
          NewValue[3] = (NewValue[3]+Value[3])/2;
          channelData[3] = map(NewValue[3],PWM_CHANNEL_MIN, PWM_CHANNEL_MAX, CRSF_CHANNEL_MIN, CRSF_CHANNEL_MAX);
          //channelData[3] = CRSF_CHANNEL_MID;
          ValChanged[3] = false;
        }

    #endif
    
    #ifdef PPM_TO_USBHID
      if(ppmReader.isReceiving()==true){
        int count = 0;
        while(ppmReader.get(count)!=0){
          int value = ppmReader.get(count);
          channelData[count] = map(value, PWM_CHANNEL_MIN, PWM_CHANNEL_MAX, CRSF_CHANNEL_MIN, CRSF_CHANNEL_MAX);
          count++;
        }
        count = 0;
        
      }

    #endif  

 // }

    Joystick.setXAxis(channelData[0]);
    Joystick.setYAxis(channelData[1]);
    //Joystick.setZAxis(channelData[2]);
    Joystick.setRxAxis(channelData[2]);
    Joystick.setRyAxis(channelData[3]);
    //Joystick.setRzAxis(channelData[5]);

    currentMicros = micros();
    Joystick.sendState();
  }


  #ifdef DEBUG
    //if(millis()-currentMillis > SECOND ){
      //Serial.println(packetPerSec); 
    //  currentMillis=millis();
    //  packetPerSec=0;
   // }
    if (micros() - serialPrintMillis >= 20000) {
      Serial<<channelData[0]<<" - "<<channelData[1]<<" - "<<channelData[2]<<" - "<<channelData[3]<<" - "<<channelData[4]<<" - "<<channelData[5]<<" - "<<channelData[6]<<" - "<<channelData[7]<<"\r\n";
      serialPrintMillis = micros();
    }
  #endif

  
}


//This is the interruption routine fro PWM
//----------------------------------------------
#ifdef PWM_TO_USBHID
  void isr1()
  {
    if (portRead(CHAN1)) Time[0] = micros();
    else if (micros() > Time[0])
    {
      Value[0] = (Value[0]+(micros()-Time[0]))/2;
      ValChanged[0] = true;
    }
  }

  void isr2()
  {
    if (portRead(CHAN2)) Time[1] = micros();
    else if (micros() > Time[1])
    {
      Value[1] = (Value[1]+(micros()-Time[1]))/2;
      ValChanged[1] = true;
    }
  }

  void isr3()
  {
    if (portRead(CHAN3)) Time[2] = micros();
    else if (micros() > Time[2])
    {
      Value[2] = (Value[2]+(micros()-Time[2]))/2;
      ValChanged[2] = true;
    }
  }

  void isr4()
  {
    if (portRead(CHAN4)) Time[3] = micros();
    else if (micros() > Time[3])
    {
      Value[3] = (Value[3]+(micros()-Time[3]))/2;
      ValChanged[3] = true;
    }
  }
#endif