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
#ifndef ARDUINO_JTAG
#define ARDUINO_JTAG

#include <Arduino.h>


class ArduinoJTAG 
{
private:
    bool tdo_;

protected:    

    /* All pins on PORTB!
     */
    enum {
        PIN_TMS  = 8,
        PIN_TDI  = 9,
        PIN_TDO  = 10,
        PIN_TCK  = 11,
        PIN_MCLR = 12
    };
    enum {
        BIT_TMS  = 0x01 << (PIN_TMS  - 8),
        BIT_TDI  = 0x01 << (PIN_TDI  - 8),
        BIT_TDO  = 0x01 << (PIN_TDO  - 8),
        BIT_TCK  = 0x01 << (PIN_TCK  - 8),
        BIT_MCLR = 0x01 << (PIN_MCLR - 8)
    };

    ArduinoJTAG()
    {
        ClearTMS();
        ClearTDI();
        ClearTCK();
        SetMCLR ();

        pinMode( PIN_TMS,  OUTPUT );
        pinMode( PIN_TDI,  OUTPUT );
        pinMode( PIN_TDO,  INPUT  );
        pinMode( PIN_TCK,  OUTPUT );
        pinMode( PIN_MCLR, OUTPUT );
    }

    inline bool ClockPulse(void)
    {
        ClearTCK();
//        asm( "  NOP" );
        delayMicroseconds(1);
//        tdo_ = digitalRead( PIN_TDO );
        tdo_ = PINB & BIT_TDO;
        SetTCK();
        return tdo_;
    }

    bool GetTDO() 
    {
        return tdo_;
    }

    inline void SetTMS()
    {
//        digitalWrite( PIN_TMS, HIGH );
        PORTB |= BIT_TMS;
    }

    inline void ClearTMS()
    {
//        digitalWrite( PIN_TMS, LOW );
        PORTB &= ~BIT_TMS;
    }

    inline void SetTCK()
    {
//        digitalWrite( PIN_TCK, HIGH );
        PORTB |= BIT_TCK;
    }

    inline void ClearTCK()
    {
//        digitalWrite( PIN_TCK, LOW );
        PORTB &= ~BIT_TCK;    
    }

    inline void SetMCLR()
    {
//        digitalWrite( PIN_MCLR, HIGH );
        PORTB |= BIT_MCLR;
    }

    inline void ClearMCLR()
    {
//        digitalWrite( PIN_MCLR, LOW );
        PORTB &= ~BIT_MCLR;
    }

    inline void SetTDI()
    {
//        digitalWrite( PIN_TDI, HIGH );
        PORTB |= BIT_TDI;
    }

    inline void ClearTDI()
    {
//        digitalWrite( PIN_TDI, LOW );
        PORTB &= ~BIT_TDI;
    }

};



#endif
