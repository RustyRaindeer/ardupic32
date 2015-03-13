# ardupic32
Automatically exported from code.google.com/p/ardupic32

ArduPIC32: An Arduino PIC32MX JTAG Programmer!
---------------------------------------------------
A simple PIC32MX JTAG flash programmer for Arduino. Not optimized in
any way, just usable enough for being able to successfully flash a real
booloader on the chip.

Use of the program should be pretty straightforward: After powering up
the Arduino, the PIC32 chip is automatically detected. Pressing 'h'
enables the operation and displays the help menu. Press 'e' to erase
the chip. Press 'P' to enter programming mode. Once in programming mode,
just copy-paste the .hex -file contents into the terminal window.

Note that the serial port speed is set to only 1200bps. This is due to the
inefficient implementation of the code which I have no plans on improving;
Feel free to rewrite if you wish ;-)

Below are the instructinos on how to connect Arduino to PIC32MX.

Arduino | Connect via | PIC32MX
---------------------------------------
PIN 8 (B0) | voltage divider | TMS
PIN 9 (B1) | voltage divider | TDI
PIN 10 (B2) | straight wire | TDO
PIN 11 (B3) | voltage divider | TCK
PIN 12 (B4) | 470ohm resistor | MCLR (note: supposing 5V tolerant pin)

NOTE: as a voltage divider I used 1.5k + 1.0k resitors. Connect 1.5k to ground,
and its other end to PIC pin. Then connect the 1.0k between PIC pin and
Arduino pin.

NOTE2: check your PIC's datasheet if MCLR is 5V tolerant! It seems that the 5V
tolerance is not the same throughout the product line (for example,
PIC32MX210F016B has all these pins 5V tolerant and you can use straight
wires instead of voltage dividers, but for example PIC32MX795F512H has
only MCLR 5V tolerant!!!!)

Remember that the PIC32MX itself is generally not 5V tolerant!
For 3.3V alimentation I used three (3) 1N4148 diodes in series
to lower from 5V to around 3.xV, worked fine enough!

For the rest of the schematic, refer to the Recommended
Minimum Connection in the datasheet of your PIC32.
For PIC32MX1XX/2XX datasheet (61168C), this is found in Figure 2-1.

The code has been successfully tested on Arduino NG with PIC32MX210F016B and
PIC32MX795F512H. Please let me know if you try with other chips!

For more info refer to Microchip documentation:
 - 61145J: PIC32MX Flash Programming Spec
 - 61121E: PIC32 Family Reference Manual, Section 5 Flash Programming 
