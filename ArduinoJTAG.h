/*
 Copyright (c) 2012-2017, Tuomo Eljas Kaikkonen
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

/**
 * Ports and Pins for fast lowlevel access
 */
#define _TMS    &PORTB, 0x01
#define _TDI    &PORTB, 0x02
#define _TDO    &PINB,  0x04
#define _TCK    &PORTB, 0x08
#define _MCLR   &PORTB, 0x10
//#define _LED    &PORTB, 0x20 // just for debugging..

 /**
  * Pins as per Arduino digitalRead/digitalWrite/pinMode compatible pin index.
  */
enum {
    PIN_TMS  = 8,
    PIN_TDI  = 9,
    PIN_TDO  = 10,
    PIN_TCK  = 11,
    PIN_MCLR = 12
};


inline void setBIT( volatile uint8_t * PORT, uint8_t bit )
{
    *PORT |= bit;
}

inline void clearBIT( volatile uint8_t * PORT, uint8_t bit )
{
    *PORT &= ~bit;
}

inline bool getBIT( volatile uint8_t * PORT, uint8_t bit )
{
    return ((*PORT) & bit) != 0;
}


class ArduinoJTAG 
{
private:
    bool tdo_;

protected:    

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
#ifdef _LED
        setBIT(_LED);
#endif
        ClearTCK();
        //delayMicroseconds(1);
        tdo_ = digitalRead( PIN_TDO );
        //tdo_ = getBIT( _TDO );
        SetTCK();
#ifdef _LED
        clearBIT(_LED);
#endif
        return tdo_;
    }

    bool GetTDO() 
    {
        return tdo_;
    }

    inline void SetTMS()
    {
        setBIT(_TMS);
    }

    inline void ClearTMS()
    {
        clearBIT(_TMS);
    }

    inline void SetTCK()
    {
        setBIT(_TCK);
    }

    inline void ClearTCK()
    {
        clearBIT(_TCK);
    }

    inline void SetMCLR()
    {
        setBIT(_MCLR);
    }

    inline void ClearMCLR()
    {
        clearBIT(_MCLR);
    }

    inline void SetTDI()
    {
        setBIT(_TDI);
    }

    inline void ClearTDI()
    {
        clearBIT(_TDI);
    }

};


#endif
