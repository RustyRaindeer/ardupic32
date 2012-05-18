/*****************************************************************************
 * ArduPIC32: An Arduino PIC32MX JTAG Programmer!
 *****************************************************************************
 * Author:   Tuomo Kaikkonen
 * Email:    t e k ( a t ) i k i . f i
 *
 * A simple PIC32MX JTAG flash programmer for Arduino. Not optimized in
 * any way, just usable enough for being able to successfully flash a real
 * booloader on the chip.
 *
 * Use of the program should be pretty straightforward: After powering up
 * the Arduino, the PIC32 chip is automatically detected. Pressing 'h' 
 * enables the operation and displays the help menu. Press 'e' to erase
 * the chip. Press 'P' to enter programming mode. Once in programming mode, 
 * just copy-paste the .hex -file contents into the terminal window. 
 *
 * Note that the serial port speed is set to 1200bps, which is about 
 * the fastest speed supported by the current form of the code; just accept
 * it or start rewriting :-P
 *
 * Below are the instructinos on how to connect Arduino to PIC32MX. 
 * (i.e. this is what I did with my ArduinoNG + PIC32MX210F016B 
 *  combo to test):
 *
 *  Arduino      | Connect via     | PIC32
 *  ---------------------------------------
 *  PIN 8  (B0)  | straight wire   | TMS    (note: 5V tolerant pin)
 *  PIN 9  (B1)  | straight wire   | TDI    (note: 5V tolerant pin)
 *  PIN 10 (B2)  | straight wire   | TDO    (note: 5V tolerant pin)
 *  PIN 11 (B3)  | straight wire   | TCK    (note: 5V tolerant pin)
 *  PIN 12 (B4)  | 470ohm resistor | MCLR   (note: 5V tolerant pin)
 *
 * Remember that the PIC32MX itself is not 5V tolerant!
 * For 3.3V alimentation I used three (3) 1N4148 diodes in series
 * to lower from 5V to around 3.xV, worked fine enough!
 * 
 * For the rest of the schematic, refer to the Recommended 
 * Minimum Connection in the datasheet of your PIC32. 
 * For PIC32MX1XX/2XX datasheet (61168C), this is found in Figure 2-1.
 * 
 * For more info refer to Microchip documentation:
 *  61145J: PIC32MX Flash Programming Spec
 *  61121E: PIC32 Family Reference Manual, Section 5 Flash Programming
 *
 ****************************************************************************/

/*
 Copyright (c) 2012, Tuomo Eljas Kaikkonen
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met: 

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer. 
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies, 
 either expressed or implied, of the FreeBSD Project.
*/

#include "Arduino.h"
#include "Pic32JTAGDevice.h"
#include "MySerial.h"


void PrintHelp()
{
    Serial.println(F("ArduPIC32 v1.00"));
    Serial.println(F("   h    - help"));
    Serial.println(F("   p    - .hex program+verify mode"));
    Serial.println(F("   P    - .hex programming only"));
    Serial.println(F("   v    - .hex verify mode"));
    Serial.println(F("   e    - Erase flash"));
    Serial.println(F("   E    - JTAG MCHP_ERASE"));
    Serial.println(F("   d    - dump memory"));
    Serial.println(F("   x    - exit"));
}


void setup() {
  Serial.begin(1200);
}


void loop() 
{
    Pic32JTAGDevice pic32;
    uint32_t addr;
    bool exit = false;
  
    if ( !pic32.GetDeviceID() || pic32.GetDeviceID() == 0xffffffff )
    {
        Serial.println(F("PIC32 device not found!"));
        Serial.println(F("Check your wiring and try again!"));
        //while(1);
    }
    else
    {
        Serial.print(F("Detected:      "));
        Serial.print(F("PIC32MX"));
        Serial.println( pic32.GetDeviceName() );
        Serial.print(F("DeviceID:      0x"));
        Serial.println( pic32.GetDeviceID(), HEX );

        Serial.print(F("Row size:      "));
        Serial.print( pic32.GetRowSize() );
        Serial.println(F("B"));
        Serial.print(F("Page size:     "));
        Serial.print( pic32.GetPageSize() );
        Serial.println(F("B"));

        Serial.print(F("Boot Flash:    0x"));
        Serial.print( pic32.GetBootFlashStart(), HEX );
        Serial.print(F(" - 0x"));
        Serial.println( pic32.GetBootFlashEnd(), HEX );

        Serial.print(F("Program Flash: 0x"));
        Serial.print( pic32.GetProgramFlashStart(), HEX );
        Serial.print(F(" - 0x"));
        Serial.println( pic32.GetProgramFlashEnd(), HEX );
        Serial.println();
        
        Serial.print(F("Press \"H\" to start!"));
        Serial.println();
    }
    
    char cmd = '!';
    while ( cmd != 'H' && cmd != 'h' )
    {
        cmd = RXChar();
    }

    PrintHelp();

    while ( !exit )
    {
        Serial.print(F(" >"));
        cmd = RXChar();
        Serial.println(cmd);

        switch (cmd)
        {

            case 'n':
                Serial.println(F("No program (dummy) mode"));
                HexPgm( pic32, false, false );
                Serial.println(F("."));
                break;

            case 'P':
                Serial.println(F("Program mode"));
                HexPgm( pic32, true, false );
                Serial.println(F("."));
                break;

            case 'p':
                Serial.println(F("Program+verify mode"));
                HexPgm( pic32, true, true );
                Serial.println(F("."));
                break;

            case 'v':
                Serial.println(F("Verify mode"));
                HexPgm( pic32, false, true );
                Serial.println(F("."));
                break;

            case 'e':
                Serial.println(F("Erase"));
                pic32.FlashOperation( NVMOP_NOP,  0x00000000, 0 );
                
                addr = pic32.GetBootFlashStart();
                while ( addr < pic32.GetBootFlashEnd() )
                {
                    Serial.print( F("Erasing: ") );
                    Serial.println( addr, HEX );
                    pic32.FlashOperation( NVMOP_ERASE_PAGE, addr, 0 );
                    pic32.FlashOperation( NVMOP_NOP, 0, 0 );

                    addr += pic32.GetPageSize();
                }

                addr = pic32.GetProgramFlashStart();
                Serial.print( F("Erasing: ") );
                Serial.println( addr, HEX );
                pic32.FlashOperation( NVMOP_ERASE_PFM, addr, 0 );
                pic32.FlashOperation( NVMOP_NOP, 0, 0 );
                  
                Serial.println(F(" - Done!"));
                break;
                
            case 'E':
                Serial.print(F("Erase"));
                pic32.ExitPgmMode();
                delay(100);
                pic32.CheckStatus();
                pic32.Erase();
                pic32.EnterPgmMode();
                Serial.println(F(" - Done!"));
                break;

            case 't':
                // Test
                Serial.println(F("Test flashing "));
                pic32.DownloadData( 0, 0x12345678 );
                pic32.DownloadData( 4, 0x9abcdef0 );
                pic32.DownloadData( 8, 0xdeadf00d );
                pic32.DownloadData( 12, 0xb00bbabe );
                pic32.FlashOperation( NVMOP_WRITE_ROW,  pic32.GetProgramFlashStart(), 0 );
                pic32.DownloadData( 0, 0xaabbccdd );
                pic32.FlashOperation( NVMOP_WRITE_WORD, pic32.GetBootFlashStart(), 0 );
                pic32.DownloadData( 0, 0x99669966 );
                pic32.FlashOperation( NVMOP_WRITE_WORD, pic32.GetBootFlashStart()+4, 0 );
                pic32.DownloadData( 0, 0xdead );
                pic32.FlashOperation( NVMOP_WRITE_WORD, pic32.GetBootFlashStart()+8, 0 );
                pic32.DownloadData( 0, 0xf00d0000 );
                pic32.FlashOperation( NVMOP_WRITE_WORD, pic32.GetBootFlashStart()+12, 0 );
                Serial.println(F("- done! "));
                break;

            case 'd':
                // Dump some locations from memmory for testing
                Serial.println(F("DevID: "));
                pic32.DumpMemory( 0xbf80f220, 1  );

                Serial.println(F("Boot Flash Start: "));
                pic32.DumpMemory( pic32.GetBootFlashStart(), 4  );

                Serial.println(F("Config bits: "));
                pic32.DumpMemory( pic32.GetConfigurationMemStart(), 4 );

                Serial.println(F("Program Flash Start: "));
                pic32.DumpMemory( pic32.GetProgramFlashStart(), 4  );

                break;


            case 'h':
            case 'H':
                PrintHelp();
                break;

            case 'x':
            case 'X':
            case 'q':
            case 'Q':
                exit = true;
                break;

        }       
    }

    pic32.ExitPgmMode();

    Serial.println(F("THE END!"));
    while ( 1 )
    {
        delay(1000); 
    } 
}


