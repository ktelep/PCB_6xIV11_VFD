6-Digit IV-11 Tube Driver Board
=============
**Description:**
This PCB is for driving 6 x IV-11 VFD 7-segment display tubes as a general purpose display using cascaded latching shift registers to control individual segments via whatever microcontroller you wish.   Connectivity is via a 7 pin header.

![PCB Image](VFDDriver2.png?raw=true "Title")

## Inputs
Power Inputs:
* 1.5V Filament Voltage
* 20V Anode and Grid Voltage
* 5V Logic Voltage
* Ground

Signal Inputs:
* Latch
* Clock
* Serial Data

## Components
* 6 x 74HC595 IC - U1,U2,U3,U4,U9,U10
* 6 x ULN2003A IC - U5,U6,U7,U8,U11,U12
* 6 x 10K 9-pin Resistor Network - RN1-RN6
* 6 x 100nf Capacitors - C1-C6
* 6 x IV-11 VFD Tubes

## Implementation Notes
I designed this as a COVID-19 project to build a clock and learn more about PCB design and driving VFD tubes.  It is probably not perfect, but it works and is generic enough that I think that if you have interest in working with these as a display this can give you a jumpstart on your project.  Updates and suggestions are welcome.

1.  The dot of the displays is not connected in this design. (I didn't need it for my purposes and would have required more complex routing and an additional ULN2003A)
2.  Segments turn on with logic LOW
3.  Be sure to pass 8 bits per digit to the shift register (see example Arduino code)
4.  There is **no power conversion** on this board, you are expected to provide the appropriate 1.5V, 5V, and 20V on the 7-pin header.   I used off the shelf buck and boost converters from eBay.

## Contact
* Kurt Telep
* e-mail: ktelep@gmail.com



