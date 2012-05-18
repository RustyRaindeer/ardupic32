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

#ifndef INCLUDE_PIC32_JTAG_DEVICE_H
#define INCLUDE_PIC32_JTAG_DEVICE_H

#include "Pic32JTAG.h"
#include "Pic32.h"
#include <avr/pgmspace.h>

enum nvmop_e {
    NVMOP_NOP        = 0,
    NVMOP_WRITE_WORD = 1,
    NVMOP_WRITE_ROW  = 3,
    NVMOP_ERASE_PAGE = 4,
    NVMOP_ERASE_PFM  = 5,   // Erase whole Program Flash Memory (PFM)
};

class Pic32JTAGDevice: Pic32JTAG {
private:
    uint32_t DeviceID_;
    struct Pic32DevID_t DevID_;

public:

    uint32_t GetDeviceID()
    {
        return DeviceID_;
    }

    char* GetDeviceName()
    {
        return DevID_.DevName;
    }

    uint16_t GetRowSize()
    {
        return DevID_.RowSize * 4;
    }

    uint16_t GetPageSize()
    {
        return DevID_.RowSize * 4 * 8;
    }

    uint16_t GetBootFlashMemorySize()
    {
        return DevID_.BFMSize * 1024;
    }

    uint16_t GetProgramFlashMemorySize()
    {
        return DevID_.PFMSize * 1024;
    }

    uint32_t GetBootFlashStart()
    {
        return 0x1FC00000;
    }

    uint32_t GetBootFlashEnd()
    {
        return 0x1FC00000 + GetBootFlashMemorySize() - 1;
    }

    uint32_t GetProgramFlashStart()
    {
        return 0x1D000000;
    }

    uint32_t GetProgramFlashEnd()
    {
        return 0x1D000000 + GetProgramFlashMemorySize() - 1;
    }

    uint32_t GetConfigurationMemStart()
    {
        return GetBootFlashEnd() + 1 - 4*4;
    }

    void CheckStatus( )
    {
      uint32_t mystatus = 0;

      SetReset(true);
      SetMode("SetMode",6, 0x1f);
      SendCommand(MTAP_SW_MTAP);
      SendCommand(MTAP_COMMAND);
      mystatus = XferData(MCHP_STATUS);

      while ( ((mystatus & 0x8) == 0) || (mystatus & 0x04) )
      {
        delay(1000); 
        mystatus = XferData(MCHP_STATUS);
      }
    }

    void Erase(  )
    {
      uint32_t mystatus = 0;

      SendCommand(MTAP_SW_MTAP);
      SendCommand(MTAP_COMMAND);
      mystatus = XferData(MCHP_ERASE);
      delay(1000);

      mystatus = 0;
      while ( ((mystatus & 0x8) == 0) || (mystatus & 0x04) )
      {
        delay(1000); 
        mystatus = XferData(MCHP_STATUS);
      }
    }

    void EnterPgmMode(  )
    {
      uint32_t mystatus = 0;

      SendCommand(MTAP_SW_MTAP);
      SendCommand(MTAP_COMMAND);
      mystatus = XferData(MCHP_STATUS);

      SendCommand(MTAP_SW_ETAP);
      SendCommand(ETAP_EJTAGBOOT);
      SetReset(false);
    }


    void ExitPgmMode()
    {
      uint32_t mystatus = 0;

      SetMode("SetMode", 5, 0x1f);
      SetReset(true);
    }


    void DownloadData( uint16_t ram_addr, uint32_t data )
    {      
            // lui s0, 0xa000
        XferInstruction( 0x3c10a000 );
            // ori s0, 0
        XferInstruction( 0x36100000 );
            // lui t0, <DATA(31:16)>
        XferInstruction( 0x3c080000 + (data>>16) );
            // ori t0, <DATA(15:0)>
        XferInstruction( 0x35080000 + (data&0xffff) );
            // sw t0, <OFFSET>(S0)
        XferInstruction( 0xae080000 + ram_addr );

            // 
            // Copy the same data to NVMDATA register for WRITE_WORD method
            // 

            // lui a0, 0xbf80
        XferInstruction( 0x3c04bf80 );
            // ori a0, 0xf400
        XferInstruction( 0x3484f400 );
            // sw t0, 48(a0)
        XferInstruction( 0xac880030 );
    }


    uint32_t FlashOperation( unsigned char nvmop, uint32_t flash_addr, unsigned int ram_addr )
    {
            // nop
        XferInstruction( 0x00000000 );

            // FROM PIC32MX flash programming specification 61145J
            // Step 1: Initialize constants
            // lui a0, 0xbf80
        XferInstruction( 0x3c04bf80 );
            // ori a0, 0xf400
        XferInstruction( 0x3484f400 );
            // ori a1, $0, 0x4000 + NVMOP<3:0>
        XferInstruction( 0x34054000 + nvmop );
            // ori a2, $0, 0x8000
        XferInstruction( 0x34068000 );
            // ori a3, $0, 0x4000
        XferInstruction( 0x34074000 );
            // lui s1, 0xaa99
        XferInstruction( 0x3c11aa99 );
            // ori s1, 0x6655
        XferInstruction( 0x36316655 );
            // lui s2, 0x5566
        XferInstruction( 0x3c125566 );
            // ori s2, 0x99aa
        XferInstruction( 0x365299aa );
            // lui s3, 0xff20
        //XferInstruction( 0x3c13ff20 );

            // Step 2, set NVMADDR (row to be programmed)
            // lui t0, <FLASH_ROW_ADDR(31:16)>
        XferInstruction( 0x3c080000 + (flash_addr>>16) );
            // ori t0, <FLASH_ROW_ADDR(15:0)>
        XferInstruction( 0x35080000 + (flash_addr&0xffff) );
            // sw t0, 32(a0)
        XferInstruction( 0xac880020 );

            // Step 3, set NVMSRCADDR (source RAM addr)
            // lui s0, <RAM_ADDR(31:16)>
        XferInstruction( 0x3c100000 );
            // ori s0, <RAM_ADDR(15:0)>
        XferInstruction( 0x36100000 + ram_addr );
            // sw s0, 64($a0)    
        XferInstruction( 0xac900040 );

            // Step 4, Set up NVMCON write and poll STAT
            // sw a1, 0(a0)
        XferInstruction( 0xac850000 );
            // <here1>
            // lw t0, 0(a0)
        XferInstruction( 0x8c880000 );
            // andi t0, 0x0800
        XferInstruction( 0x31080800 );
            // bne t0, $0, <here1>   
        XferInstruction( 0x1500fffd );
            // nop
        XferInstruction( 0x00000000 );

            // Step 5, unlock NVMCON and start write
            // sw s1, 16(a0)
        XferInstruction( 0xac910010 );
            // sw s2, 16(a0)
        XferInstruction( 0xac920010 );
            // sw a2, 8(a0)  NOTE: offset 8 here is the "set register"
        XferInstruction( 0xac860008 );

            // Step 6, Poll for NVMCON(WR) bit to get cleared
            // <here2>
            // lw t0, 0(a0)
        XferInstruction( 0x8c880000 );
            // and t0, t0, a2
        XferInstruction( 0x01064024 );
            // bne t0, $0, <here2>
        XferInstruction( 0x1500fffd );
            // nop
        XferInstruction( 0x00000000 );

            // Step 7, Wait at least 500ns, 8MHz clock assumed
            // nop
        XferInstruction( 0x00000000 );
            // nop
        XferInstruction( 0x00000000 );
            // nop
        XferInstruction( 0x00000000 );
            // nop
        XferInstruction( 0x00000000 );

            // Step 8, Clear NVMCON(WREN) bit
            // sw a3, 4(a0)  NOTE: Offset 4 here is the "clear" register
        XferInstruction( 0xac870004 );

            // Step 9, Check NVMCON(WRERR) bit to ensure correct operation
            // lw t0, 0(a0)
        XferInstruction( 0x8c880000 );
            // andi t0, $0, 0x2000
//        XferInstruction( 0x30082000 );
 
            // lui s3, 0xFF20
        XferInstruction( 0x3c13ff20 );
            // ori s3, 0
        XferInstruction( 0x36730000 );        
            // sw t0, 0(s3) = transfer NVMCON via fastdata
        XferInstruction( 0xae680000 );
                
            // bne t0, $0, <err_proc_offset>
    //    XferInstruction( 0x15000000 + err_proc_offs );

            // nop
        XferInstruction( 0x00000000 );

        SendCommand( ETAP_FASTDATA );
        return XferFastData( 0 );
    }


    uint32_t ReadFlashData( uint32_t flash_addr )
    {                
            // lui s3, 0xFF20
        XferInstruction( 0x3c13ff20 );
            // ori s3, 0
        XferInstruction( 0x36730000 );        
            // lui t0, <FLASH_WORD_ADDR(31:16)>
        XferInstruction( 0x3c080000 + (flash_addr>>16) );
            // ori t0, <FLASH_WORD_ADDR(15:0)>
        XferInstruction( 0x35080000 + (flash_addr&0xffff) );
            // lw t1, 0(t0)
        XferInstruction( 0x8d090000 );  
            // sw t0, 0(s3) = transfer flash_addr back for debugging
        //XferInstruction( 0xae680000 );
            // sw t1, 0(s3)
        XferInstruction( 0xae690000 );
            // NOTE: Now, the CPU has written to FASTDATA 
            //       area 0xff200000. The PrAcc flag is 
            //       clear (0); if we do two writes to fastdata,
            //       prAcc is set since the second write remains
            //       stalled until the first XferFastData is executed.
        SendCommand( ETAP_FASTDATA );
        //uint32_t addr = XferFastData( 0 );
        //Serial.println( addr, HEX );
        return XferFastData( 0 );
    }


    void DumpMemory( uint32_t addr, uint8_t num )
    {
        while ( num-- )
        {
            Serial.print ( addr, HEX );
            Serial.print ( F(": ") );
            Serial.println( ReadFlashData(addr), HEX );
            addr +=4 ;
        }
    }


        // 
        // Constructor
        // 
    Pic32JTAGDevice()
    {
        int n = 0;
        DeviceID_ = 0;

        CheckStatus();
        //Erase();
        EnterPgmMode();

        DeviceID_ = ReadFlashData(0xbf80f220);

/*
     TEST
        DevID_.DevID   = 0x4A01053;
        DevID_.DevName[0] = '2';
        DevID_.DevName[1] = '1';
        DevID_.DevName[2] = '0';
        DevID_.DevName[3] = 'F';
        DevID_.DevName[4] = '0';
        DevID_.DevName[5] = '1';
        DevID_.DevName[6] = '6';
        DevID_.DevName[7] = 'B';
        DevID_.DevName[8] = 0;
        DevID_.RowSize = 32;
        DevID_.BFMSize = 3;
        DevID_.PFMSize = 16;
*/
            //
            // Find the device from Pic32DevIDList
            //
        DevID_.DevID = 0xFFFFFFFF;
        while ( DeviceID_ != DevID_.DevID &&
                0         != DevID_.DevID )
        {
            memcpy_P( &DevID_, &Pic32DevIDList[n], sizeof(DevID_) );
            ++n;
        }
    }
};

#endif //INCLUDE_PIC32_JTAG_DEVICE_H
