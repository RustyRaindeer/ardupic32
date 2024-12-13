ArduPIC32: An Arduino PIC32MX JTAG Programmer!
---------------------------------------------------
ArduPIC32, a simple PIC32MX JTAG flash programmer for
Arduino. Provides slow programming speed, but still enough for
successfully flashing a real booloader on the chip.

Use of the program should be pretty straightforward: After powering up
the Arduino, the PIC32 chip is automatically detected. Pressing 'h'
enables the operation and displays the help menu. Press 'e' to erase
the chip. Press 'P' to enter programming mode. Once in programming
mode, just copy-paste the .hex -file contents into the terminal
window.

Note that the serial port speed is limited to only 1200bps. JTAG
protocol is created by bit banging the PORTB register directly. Not
making use of Arduino digitalWrite method, since it proved too slow
for the purpose.  If using on a different Arduino than my old NG, you
should probably check and modify ArduinoJTAG.h accordingly.

Below are the instructinos on how to connect Arduino to PIC32MX.

![JTAG_interface](https://github.com/user-attachments/assets/d49b4de7-1c0b-466f-a169-3e2ee311cbd3)

NOTE: check your PIC's datasheet if MCLR is 5V tolerant! It seems that the 5V
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
