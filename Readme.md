CRSF decode library for arduino atmega32u4/328p.

Based on arduino SBUS decode, modified to decode crsf protocol from elrs receiver

https://github.com/mikeshub/FUTABA_SBUS/tree/master/FUTABA_SBUS

The example convert elrs reciver to USB HID via serial use this library:

https://github.com/MHeironimus/ArduinoJoystickLibrary

Could modify for other usage (output pwm to drive servo)

There is a better impementation of crsf lib, check out :

https://github.com/CapnBry/CRServoF


Used for simulator, tested on velocity drone/Tinywhoop Go/VRC Pro, and on Skydievr (android version use otg)

### Update 2022/07/13

Add PWM and PPM support

Uncomment define to select protocol:

#define CRSF_TO_USBHID

//#define PWM_TO_USBHID

//#define PPM_TO_USBHID

### Wiring

ELRS receiver  -  Arduino Pro Micro

5v  -> 5v

GND -> GND

RX  -> TX

TX  -> RX



Due to the hardware limitation, the receiver need to work at 115200 baud rate (Default is 420k), the latest elrs configrator support customer baud rate.

![屏幕截图 2022-04-30 135045](https://user-images.githubusercontent.com/43392862/166093376-be980072-038f-40fe-8a93-302f23605fc6.jpg)


The library could be work independly for other usage.


PWM receiver - Arduino Pro Micro

5v  -> 5v

GND -> GND

Channel 1  -> TX (D0)

Channel 2  -> RX (D1)

Channel 3  -> D2

Channel 4  -> D3

PPM receiver - Arduino Pro Micro

GND -> GND

Siginal  -> D3

![微信图片_20220427230109](https://user-images.githubusercontent.com/43392862/166093331-778e0137-e148-4e79-9ff4-059480c27bcf.jpg)






