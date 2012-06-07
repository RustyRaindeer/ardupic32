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

#ifndef INCLUDE_PIC32_JTAG_H
#define INCLUDE_PIC32_JTAG_H

#include "ArduinoJTAG.h"

#define MTAP_COMMAND "MTAP_COMMAND",5,0x07
#define MTAP_SW_MTAP "MTAP_SW_MTAP",5,0x04
#define MTAP_SW_ETAP "MTAP_SW_ETAP",5,0x05
#define MTAP_IDCODE   "MTAP_IDCODE",5,0x01

#define ETAP_ADDRESS     "ETAP_ADDRESS",5,0x08
#define ETAP_DATA           "ETAP_DATA",5,0x09
#define ETAP_CONTROL     "ETAP_CONTROL",5,0x0A
#define ETAP_EJTAGBOOT "ETAP_EJTAGBOOT",5,0x0C
#define ETAP_FASTDATA   "ETAP_FASTDATA",5,0x0E

#define MCHP_STATUS               "MCHP_STATUS",8,0x00
#define MCHP_ASERT_SRT         "MCHP_ASERT_SRT",8,0x01
#define MCHP_DE_ASERT_SRT   "MCHP_DE_ASERT_SRT",8,0x00
#define MCHP_ERASE                 "MCHP_ERASE",8,0xFC
#define MCHP_FLASH_ENABLE   "MCHP_FLASH_ENABLE",8,0xFE
#define MCHP_FLASH_DISABLE "MCHP_FLASH_DISABLE",8,0xFD

bool _debug = 0;

class Pic32JTAG: public ArduinoJTAG {

private:
    bool prAcc_;

protected:
    Pic32JTAG()
    {
        
    }

public:
    void SetReset(bool set)
    {
        if ( set )
            ClearMCLR();
        else
            SetMCLR();
    }

protected:
    uint32_t SetMode(char* cmdname, unsigned char bits, uint32_t mode)
    {
      uint32_t data = 0;
      if (_debug) Serial.println(cmdname);

      ClearTDI();

      unsigned char bitnum = 0;
      while (bits--)
      {
        if ( mode & 0x01 )
        {
            SetTMS();
        }
        else
        {
            ClearTMS();
        }
        mode >>= 1;

        bool tdo = ClockPulse();
        data |= (uint32_t)tdo << bitnum;
        ++bitnum;
      }

      if (_debug) Serial.println(data, HEX);
      return data;
    }

    void SendCommand(char* cmdname, unsigned char bits, uint32_t cmd)
    {
      uint32_t data = 0;
      if (_debug) Serial.println(cmdname);

      ClearTDI();
      SetTMS();
      ClockPulse();
      ClockPulse();
      ClearTMS();
      ClockPulse();
      ClockPulse();

      unsigned char bitnum = 0;
      while ( bits-- )
      {
          if ( !bits )
          {
              SetTMS();
          }

          if ( cmd & 0x01 )
          {
              SetTDI();
          }
          else
          {
              ClearTDI();
          }
          cmd >>= 1;

          bool tdo = ClockPulse();
          data |= (uint32_t)tdo << bitnum;
          bitnum++;
      }

      ClearTDI();
      ClockPulse();
      ClearTMS();
      ClockPulse();  

      if (_debug) Serial.println(data, HEX);
    }

    uint32_t XferDataData(unsigned char bits, uint32_t cmd)
    {
      uint32_t data = 0;  
      unsigned char bitnum = 0;
      while ( bits-- )
      {
        if ( !bits )
        {
            SetTMS();
        }

        if ( cmd & 0x01 )
        {
            SetTDI();
        }
        else
        {
            ClearTDI();
        }
        cmd >>= 1;

        bool tdo = ClockPulse();
        data |= (uint32_t)tdo << bitnum;
        bitnum++;
      }
      return data;
    }


    uint32_t XferData(unsigned char bits, uint32_t cmd)
    {
      uint32_t data = 0;

      ClearTDI();
      SetTMS();
      ClockPulse();
      ClearTMS();
      ClockPulse();
      ClockPulse();

      data = XferDataData(bits, cmd);

      ClearTDI();
      ClockPulse();
      ClearTMS();
      ClockPulse();  

      return data;
    }

    uint32_t XferFastData(uint32_t cmd)
    {
      uint32_t data = 0;

      if (_debug) Serial.print("XferFastData ");
      if (_debug) Serial.println(cmd, HEX);

      do
      {
          SetTMS();
          ClockPulse();
          ClearTMS();
          ClockPulse();
          ClockPulse();

          ClearTDI();
          prAcc_ = ClockPulse();  
          data = XferDataData(32, cmd);

          ClearTDI();
          ClockPulse();
          ClearTMS();
          ClockPulse();

      } while(0);

      return data;
    }

    bool getPrAcc(void)
    {
        return prAcc_;
    }

    uint32_t XferData(char* cmdname, unsigned char bits, uint32_t cmd)
    {
      uint32_t data = 0;
      if (_debug) Serial.println(cmdname);
      data = XferData( bits, cmd );
      if (_debug) Serial.println(data, HEX);
      return data;
    }


    void XferInstruction(uint32_t instr)
    {
      uint32_t controlVal = 0;
      if (_debug) Serial.print("XferInstruction 0x");
      if (_debug) Serial.println(instr, HEX);

      SendCommand(ETAP_CONTROL);
      do
      {
        controlVal = XferData(32, 0x0004C000);
        if ((controlVal & 0x00040000) == 0)
        {
            delay(1000);
        }
      } while ( (controlVal & 0x00040000) == 0 );

      SendCommand(ETAP_DATA);
      XferData(32, instr);
      SendCommand(ETAP_CONTROL);
      XferData(32, 0x0000C000);
    }

};

#endif //INCLUDE_PIC32_JTAG_H
