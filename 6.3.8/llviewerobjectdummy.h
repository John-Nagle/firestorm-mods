#ifndef LLVIEWEROBJECTDUMMY_H
#define LLVIEWEROBJECTDUMMY_H
#include <string>
class LLViewerObject                                                // dummy for testing
{
private:
    //  Dummy data from log file
    LLVector3 mTime;
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
    
    void parseline(const std::string& s);                       // get data from line of test string
};

#endif // LLVIEWEROBJECT_H
