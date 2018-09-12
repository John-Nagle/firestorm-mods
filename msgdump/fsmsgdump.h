
//
//  Formats of Second Life object update messages
//
//  These are fixed format structures, even though the code
//  in llviewerobject.cpp does not decode them that way.
//
#include "stdint.h"

//
//  Net-order types
//
typedef uint16_t U16Net;                //
typedef uint32_t LLVector3Net[3];       // an LLVector3 in net order
typedef uint32_t LLVector4Net[4];
//
//  ImprovedTerseObjectUpdate
//
//  These messages come in a number of fixed lengths, and
//  are identified only by their length. 
//
//  The 16-byte update
//
struct ImprovedTerseObjectUpdate16 {
    uint8_t position[3];                 // integer-scaled low-accuracy values
    uint8_t velocity[3];
    uint8_t acceleration[3];
    uint8_t rotation[4];
    uint8_t angularvelocity[3];   
};

//
//  The 32-byte update. Same info, larger numeric values.
//
struct ImprovedTerseObjectUpdate32 {
    U16Net position[3];                 // integer-scaled low-accuracy values
    U16Net velocity[3];
    U16Net acceleration[3];
    U16Net rotation[4];
    U16Net angularvelocity[3];   
};

//  
//  The 48-byte update. Adds a foot plane normal
//
struct ImprovedTerseObjectUpdate48 {
    LLVector4Net footplane;                 // foot plane
    ImprovedTerseObjectUpdate32 update32;   // contains a 32-byte update
};

//
//  The 60-byte update.  32 bit values
//
struct ImprovedTerseObjectUpdate60 {
    LLVector3Net position;                  // higher resolution values
    LLVector3Net velocity;
    LLVector3Net acceleration;
    LLVector3Net rotation;                  // quaternion without the fourth element?
    LLVector3Net angularvelocity;   
};

//
//  The 76-byte update.  Adds a foot plane  normal
//
struct ImprovedTerseObjectUpdate76 {
    LLVector4Net footplane;
    ImprovedTerseObjectUpdate60 update60;   // contains a 60-byte update
};

//
//  All the alternatives for ImprovedTerseObjectUpdate
//
union ImprovedTerseObjectUpdate {
    ImprovedTerseObjectUpdate16 update16;
    ImprovedTerseObjectUpdate32 update32;
    ImprovedTerseObjectUpdate48 update48;
    ImprovedTerseObjectUpdate60 update60;
    ImprovedTerseObjectUpdate76 update76;
    //  Check all sizes at compile time.
    static_assert(sizeof(ImprovedTerseObjectUpdate16) == 16, "ImprovedTerseObjectUpdate16 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ImprovedTerseObjectUpdate32) == 32, "ImprovedTerseObjectUpdate32 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ImprovedTerseObjectUpdate48) == 48, "ImprovedTerseObjectUpdate48 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ImprovedTerseObjectUpdate60) == 60, "ImprovedTerseObjectUpdate60 struct is wrong size");   // must be exactly this size    
    static_assert(sizeof(ImprovedTerseObjectUpdate76) == 76, "ImprovedTerseObjectUpdate60 struct is wrong size");   // must be exactly this size


};
