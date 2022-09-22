/* 
 * kekse23.de AVRPort23
 * Copyright (c) 2017, Nicholas Regitz
 * 
 * Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 */

/*
   portMode(port, pin, mode, state)
   portMode2(port, pin, mode)
   portRead(port, pin)
   portWrite(port, pin, value)
   portToggle(port, pin)
*/

#define __portGet(port, pin)          ((PIN##port >> pin) & 0x01)
#define __portSet(port, pin)          ((PORT##port) |= (1 << pin))
#define __portClear(port, pin)        ((PORT##port) &= ~(1 << pin))
#define __portToggle(port, pin)       ((PIN##port) |= (1 << pin))
#define __portToggle2(port, pin)      ((PORT##port) ^= (1 << pin)) //Belegt 4 byte mehr
#define __portInput(port, pin)        ((DDR##port) &= ~(1 << pin))
#define __portOutput(port, pin)       ((DDR##port) |= (1 << pin))

#define __portMode(port, pin, mode, state)  (mode ? __portOutput(port, pin) : __portInput(port, pin)) & (state ? __portSet(port, pin) : __portClear(port, pin))
#define __portModeS(port, pin, mode)  (mode ? __portOutput(port, mode) : __portInput(port, mode))
#define __portWrite(port, pin, value) (value ? __portSet(port, pin) : __portClear(port, pin))

#define portMode(args...) __portMode(args)
#define portModeS(args...) __portModeS(args)

#define portRead(args...) __portGet(args)
#define portWrite(args...) __portWrite(args)
#define portToggle(args...) __portToggle(args)

