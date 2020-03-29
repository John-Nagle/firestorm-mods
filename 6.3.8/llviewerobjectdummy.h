#ifndef LLVIEWEROBJECTDUMMY_H
#define LLVIEWEROBJECTDUMMY_H
#include <string>
//
//  Dummy types normally defined within Firestorm/LL code
//
//  For unit test only. Not needed inside viewer.
//
class LLViewerObject                                                // dummy for testing
{
public:
    //  Dummy data from log file
    F64 mTime;
    LLVector3 mPos;
    LLQuaternion mRot;
    LLVector3 mVel;
    LLVector3 mAngVel;
    std::string mRegion;
public:
    RegionCrossExtrapolate mExtrap;                                 // extrapolator
    const LLVector3 getVelocity() const { return(mVel); }                          // get basic object info
    const LLVector3& getPositionRegion() const { return(mPos); }
    const LLQuaternion getRotationRegion() const { return(mRot); }
    const LLVector3 getAngularVelocity() const { return(mAngVel); }
};

#endif // LLVIEWEROBJECT_H
