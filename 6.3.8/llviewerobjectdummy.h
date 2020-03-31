#ifndef LLVIEWEROBJECTDUMMY_H
#define LLVIEWEROBJECTDUMMY_H
#include <string>
#include <vector>
#include "fsregioncrosstestdummies.h"  // ***TEMP***
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
    
    //  Object tree, so we can create a seat->avatar relationship
    std::vector<LLViewerObject*> mChildList;
    LLViewerObject* mParent;                                        // parent object
    BOOL mIsAvatar;
    
public:
    LLViewerObject() :
    mParent(NULL),
    mIsAvatar(false)
    {}
    RegionCrossExtrapolate mExtrap;                                 // extrapolator
    const LLVector3 getVelocity() const { return(mVel); }                          // get basic object info
    const LLVector3& getPositionRegion() const { return(mPos); }
    const LLQuaternion getRotationRegion() const { return(mRot); }
    const LLVector3 getAngularVelocity() const { return(mAngVel); }
    BOOL isAvatar() const { return(mIsAvatar); }
    LLViewerObject* getParent() const { return(mParent); }
    BOOL isRoot() const { return(getParent() == NULL); }
};

#endif // LLVIEWEROBJECT_H
