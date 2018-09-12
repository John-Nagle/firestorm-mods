
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
//  ObjectUpdateData
//
//  This "data" element appears
//  inside both ObjectUpdateData and ObjectUpdate.
//  These messages come in a number of fixed lengths, and
//  are identified only by their length.  
//
//  The 16-byte update
//
struct ObjectUpdateData16 {
    uint8_t position[3];                 // integer-scaled low-accuracy values
    uint8_t velocity[3];
    uint8_t acceleration[3];
    uint8_t rotation[4];
    uint8_t angularvelocity[3];   
};

//
//  The 32-byte update. Same info, larger numeric values.
//
struct ObjectUpdateData32 {
    U16Net position[3];                 // integer-scaled low-accuracy values
    U16Net velocity[3];
    U16Net acceleration[3];
    U16Net rotation[4];
    U16Net angularvelocity[3];   
};

//  
//  The 48-byte update. Adds a foot plane normal
//
struct ObjectUpdateData48 {
    LLVector4Net footplane;                 // foot plane
    ObjectUpdateData32 update32;   // contains a 32-byte update
};

//
//  The 60-byte update.  32 bit values
//
struct ObjectUpdateData60 {
    LLVector3Net position;                  // higher resolution values
    LLVector3Net velocity;
    LLVector3Net acceleration;
    LLVector3Net rotation;                  // quaternion without the fourth element?
    LLVector3Net angularvelocity;   
};

//
//  The 76-byte update.  Adds a foot plane normal
//
struct ObjectUpdateData76 {
    LLVector4Net footplane;
    ObjectUpdateData60 update60;   // contains a 60-byte update
};

//
//  All the alternatives for ObjectUpdateData
//
union ObjectUpdateData {
    ObjectUpdateData16 update16;
    ObjectUpdateData32 update32;
    ObjectUpdateData48 update48;
    ObjectUpdateData60 update60;
    ObjectUpdateData76 update76;
    //  Check all sizes at compile time.
    static_assert(sizeof(ObjectUpdateData16) == 16, "ObjectUpdateData16 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ObjectUpdateData32) == 32, "ObjectUpdateData32 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ObjectUpdateData48) == 48, "ObjectUpdateData48 struct is wrong size");   // must be exactly this size
    static_assert(sizeof(ObjectUpdateData60) == 60, "ObjectUpdateData60 struct is wrong size");   // must be exactly this size    
    static_assert(sizeof(ObjectUpdateData76) == 76, "ObjectUpdateData60 struct is wrong size");   // must be exactly this size


};
