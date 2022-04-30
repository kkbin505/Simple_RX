CRSF decode library for arduino atmega32u4/328p.

Based on arduino SBUS decode, modified to decode crsf protocol from elrs receiver

https://github.com/mikeshub/FUTABA_SBUS/tree/master/FUTABA_SBUS

The example convert elrs reciver to USB HID via serial. 

Used for simulator, tested on velocity drone/Tinywhoop Go/VRC Pro, and on Skydievr (android version use otg)

Due to the hardware limitation, the receiver need to work at 115200 baud rate (Default is 420k), the latest elrs configrator support customer baud rate.

![屏幕截图 2022-04-30 135045](https://user-images.githubusercontent.com/43392862/166093376-be980072-038f-40fe-8a93-302f23605fc6.jpg)


The library could be work independly for other usage.


![微信图片_20220427230109](https://user-images.githubusercontent.com/43392862/166093331-778e0137-e148-4e79-9ff4-059480c27bcf.jpg)
