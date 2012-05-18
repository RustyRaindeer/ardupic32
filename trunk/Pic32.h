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

#ifndef PIC32_DEVIDS
#define PIC32_DEVIDS

#include <avr/pgmspace.h>

struct Pic32DevID_t {
    uint32_t DevID;
    char     DevName[9];
    uint8_t  RowSize;  // Row size in words. Page size = x8
    uint16_t BFMSize;  // Boot Flash Memory    (expected @ 0x1FC00000)
    uint16_t PFMSize;  // Program Flash Memory (expected @ 0x1D000000)
};

PROGMEM const Pic32DevID_t Pic32DevIDList[] =
{
   // DevID      DevName       RowSz BFMSz PFMSz
    { 0x4A07053, "110F016B", 32,  3,  16  },
    { 0x4A09053, "110F016C", 32,  3,  16  },
    { 0x4A0B053, "110F016D", 32,  3,  16  },
    { 0x4A06053, "120F032B", 32,  3,  32  },
    { 0x4A08053, "120F032C", 32,  3,  32  },
    { 0x4A0A053, "120F032D", 32,  3,  32  },
    { 0x4D07053, "130F064B", 32,  3,  64  },
    { 0x4D09053, "130F064C", 32,  3,  64  },
    { 0x4D0B053, "130F064D", 32,  3,  64  },
    { 0x4D06053, "150F128B", 32,  3,  128 },
    { 0x4D08053, "150F128C", 32,  3,  128 },
    { 0x4D0A053, "150F128D", 32,  3,  128 },
    { 0x4A01053, "210F016B", 32,  3,  16  },
    { 0x4A03053, "210F016C", 32,  3,  16  },
    { 0x4A05053, "210F016D", 32,  3,  16  },
    { 0x4A00053, "220F032B", 32,  3,  32  },
    { 0x4A02053, "220F032C", 32,  3,  32  },
    { 0x4A04053, "220F032D", 32,  3,  32  },
    { 0x4D01053, "230F064B", 32,  3,  64  },
    { 0x4D03053, "230F064C", 32,  3,  64  },
    { 0x4D05053, "230F064D", 32,  3,  64  },
    { 0x4D00053, "250F128B", 32,  3,  128 },
    { 0x4D02053, "250F128C", 32,  3,  128 },
    { 0x4D04053, "250F128D", 32,  3,  128 },
    { 0x938053,  "360F512L", 128, 12, 512 },
    { 0x934053,  "360F256L", 128, 12, 256 },
//  { 0x92D053,  "340F128L", 0,   0,  128 },  // pending: Check row size + Boot Flash size!!
    { 0x92A053,  "320F128L", 128, 12, 128 },
//  { 0x916053,  "340F512H", 0,   0,  512 },
    { 0x912053,  "340F256H", 128, 12, 256 },
//  { 0x90D053,  "340F128H", 0,   0,  128 },
    { 0x90A053,  "320F128H", 128, 12, 128 },
    { 0x906053,  "320F064H", 128, 12, 64  },
    { 0x902053,  "320F032H", 128, 12, 32  },
//  { 0x978053,  "460F512L", 0,   0,  512 },
//  { 0x974053,  "460F256L", 0,   0,  256 },
//  { 0x96D053,  "440F128L", 0,   0,  128 },
//  { 0x952053,  "440F256H", 0,   0,  256 },
//  { 0x956053,  "440F512H", 0,   0,  512 },
//  { 0x94D053,  "440F128H", 0,   0,  128 },
//  { 0x942053,  "420F032H", 0,   0,  032 },
    { 0x4307053, "795F512L", 128, 12, 512 },
    { 0x430E053, "795F512H", 128, 12, 512 },
    { 0x4306053, "775F512L", 128, 12, 512 },
    { 0x430D053, "775F512H", 128, 12, 512 },
    { 0x4312053, "775F256L", 128, 12, 256 },
    { 0x4303053, "775F256H", 128, 12, 256 },
    { 0x4417053, "764F128L", 128, 12, 128 },
    { 0x440B053, "764F128H", 128, 12, 128 },
    { 0x4341053, "695F512L", 128, 12, 512 },
    { 0x4325053, "695F512H", 128, 12, 512 },
    { 0x4311053, "675F512L", 128, 12, 512 },
    { 0x430C053, "675F512H", 128, 12, 512 },
    { 0x4305053, "675F256L", 128, 12, 256 },
    { 0x430B053, "675F256H", 128, 12, 256 },
    { 0x4413053, "664F128L", 128, 12, 128 },
    { 0x4407053, "664F128H", 128, 12, 128 },
    { 0x4411053, "664F064L", 128, 12, 64  },
    { 0x4405053, "664F064H", 128, 12, 64  },
    { 0x430F053, "575F512L", 128, 12, 512 },
    { 0x4309053, "575F512H", 128, 12, 512 },
    { 0x4333053, "575F256L", 128, 12, 256 },
    { 0x4317053, "575F256H", 128, 12, 256 },
    { 0x440F053, "564F128L", 128, 12, 128 },
    { 0x4403053, "564F128H", 128, 12, 128 },
    { 0x440D053, "564F064L", 128, 12, 64  },
    { 0x4401053, "564F064H", 128, 12, 64  },
    { 0x4400053, "534F064H", 128, 12, 64  },
    { 0x440C053, "534F064L", 128, 12, 64  },
    { 0,         "Unknown",  0,   0,  0   }
};

#endif

