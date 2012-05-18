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

#ifndef _MY_SERIAL_H
#define _MY_SERIAL_H

#include <Arduino.h>
#include "Pic32JTAGDevice.h"

uint16_t GlobalCheckSum = 0;


char RXChar(void)
{
    // Wait for inc character
    while(!Serial.available())
    {
        asm(" nop");
    }

    return Serial.read();
}


unsigned char Ascii2Hex(unsigned char a)
{
    if (a >= '0' && a <='9')
    {
        return a-'0';
    }
    else if (a >= 'A' && a <='F')
    {
        return 10+(a-'A');
    }
    else if (a >= 'a' && a <='f')
    {
        return 10+(a-'a');
    }
    else
    {
        return 0;
    }
}

unsigned char RXAsciiByte(void)
{
    unsigned char b1,b2;

    b1 = RXChar();
    b2 = RXChar();

    b1 = Ascii2Hex(b1);
    b2 = Ascii2Hex(b2);
    b1 = ((b1<<4) + b2);
    return b1;
}

uint16_t RXAsciiWord(void)
{
    unsigned char b1,b2;

    b1 = RXAsciiByte();
    b2 = RXAsciiByte();

    return ((((uint16_t)b1)<<8) + b2);
}


void ConsumeRestOfFile( void )
{
    uint16_t phase = 0;
  
    Serial.println (F("\nPlease wait, throwing away the rest of the file."));

    do
    {
        while ( Serial.available() > 0 )
            Serial.read();

        Serial.print(F("\x1b[1;0H"));  // goto row 1, column 0
        switch ((phase++)&0x3)
        {
            case 0: Serial.print(F("-"));  break;
            case 1: Serial.print(F("\\")); break;
            case 2: Serial.print(F("|"));  break;
            case 3: Serial.print(F("/"));  break;
        }

        delay(1000);
    } while( Serial.available() > 0 );
    
    Serial.println();
    Serial.println();
    Serial.println();
}


void HexPgm( Pic32JTAGDevice pic32, bool program, bool verify )
{
    uint32_t flashAddr;
    uint8_t  data[128];
    uint16_t line = 0;

    uint16_t startCode = 0x0a;
    uint16_t byteCount;
    uint32_t address;
    uint32_t addressHi = 0;
    uint16_t recordType = 0;

    uint16_t i;
    uint16_t Status;
    uint16_t phase = 0;

    Serial.println (F("Send your .hex -file now."));

    while (recordType != 01)  // end
    {
        ++line;
        do
        {
            startCode = RXChar();
            GlobalCheckSum = 0;
            if ( line == 1 )
            {
                // ANSI clear screen
                Serial.print(F("\x1b[2J\x1b[0;0H"));
            }
        } while (startCode <= ' ');

        if (startCode != ':')
        {   
            Serial.print(F("Start code fail!"));    

            // error
            ConsumeRestOfFile();
            return;
        }
      
        Serial.print(F("\x1b[1;0H"));  // goto row 1, column 0
        switch ((phase++)&0x3)
        {
            case 0: Serial.print(F("-")); break;
            case 1: Serial.print(F("\\")); break;
            case 2: Serial.print(F("|")); break;
            case 3: Serial.print(F("/")); break;
        }
      
        byteCount  = (uint16_t)RXAsciiByte();
        address    = (uint32_t)RXAsciiWord();
        recordType = (uint16_t)RXAsciiByte();

        switch (recordType)
        {
            case 0:
                for (i = 0; i < byteCount; ++i)
                {
                    data[i] = RXAsciiByte();
                }
                RXAsciiByte(); /* checksum */

                if ((GlobalCheckSum&0x00FF) == 0) 
                {   
                    flashAddr = (addressHi<<16) + (address);

                    if ( program )
                    {
                        uint32_t *curData;
                        uint32_t  byte = 0;
                        uint32_t  pgmAddr = flashAddr;
                        curData = (uint32_t*)(data);
                        
                        while ( byte < byteCount )
                        {
                            //Serial.print (F("Flashing 0x"));Serial.print(pgmAddr, HEX);
                            //Serial.print (F(" = 0x"));Serial.println( (*curData), HEX);
                            pic32.DownloadData( 0, (*curData) );
                            pic32.FlashOperation( NVMOP_WRITE_WORD, pgmAddr, 0 );
                            byte += 4;
                            pgmAddr += 4;
                            ++curData;
                        }    
                    }

                    if ( verify )
                    {
                        uint32_t *curData;
                        int16_t   byte = 0;
                        curData = (uint32_t*)(data);
                        while ( byte < byteCount )
                        {
                            //Serial.print (F("Verifying 0x"));Serial.print(flashAddr, HEX);
                            uint32_t fdata = pic32.ReadFlashData( flashAddr );
                            if ( (*curData) != fdata )
                            {
                                Serial.print (F("Verify failed at 0x"));
                                Serial.println ( flashAddr, HEX );
                                Serial.print ( F(" 0x"));
                                Serial.print ( fdata, HEX );
                                Serial.print ( F(" <> 0x") );
                                Serial.println ( (*curData), HEX );             
                                ConsumeRestOfFile();
                                return;
                            }
                            byte += 4;
                            flashAddr += 4;
                            ++curData;
                        }                        
                        
                    }
                }

                break;

            case 1:
                //end
                RXAsciiByte();  /*checksum */
                break;

            case 4:
                addressHi = RXAsciiWord();
                RXAsciiByte();  /* checksum */

                break;

            default:
                Serial.println(F("HEXfile error!"));
                ConsumeRestOfFile();
                return;
    
        }

        if ((GlobalCheckSum&0x00FF) != 00) 
        {   
            // checksum error
            Serial.print(F("Chksum fail line "));
            Serial.println(line);
            ConsumeRestOfFile();
            return;
        }
    }
    
    Serial.println(F(""));
    Serial.println(F("Done!"));
}


#endif

