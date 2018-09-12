
//
//  Formats of Second Life object update messages
//
//  These are fixed format structures, even though the code
//  in llviewerobject.cpp does not decode them that way.
//
#include "stdint.h"
//
//  ImprovedTerseObjectUpdate
//
//  These messages come in a number of fixed lengths, and
//  are identified only by their length. 
//
//  The 16-byte update

struct ImprovedTerseObjectUpdate16 {
    uint8_t position[3];                 // integer-scaled low-accuracy values
    uint8_t velocity[3];
    uint8_t acceleration[3];
    uint8_t rotation[4];
    uint8_t angularvelocity[3];   
};

union ImprovedTerseObjectUpdate {
    ImprovedTerseObjectUpdate16 update16;
    static_assert(sizeof(ImprovedTerseObjectUpdate16) == 16, "ImprovedTerseObjectUpdate is wrong size");   // must be exactly this size
};
