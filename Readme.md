CRSF decode library for arduino atmega32u4/328p.

Based on arduino SBUS decode, modified to decode crsf protocol from elrs receiver

https://github.com/mikeshub/FUTABA_SBUS/tree/master/FUTABA_SBUS

The example convert elrs reciver to USB HID via serial. 

Used for simulator, tested on velocity drone/Tinywhoop Go/VRC Pro, and on Skydievr (android version use otg)

Due to the hardware limitation, the receiver need to work at 115200 baud rate (Default is 420k), the latest elrs configrator support customer baud rate.

The library could be work independly for other usage.


