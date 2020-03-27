//
//  Unit test for fsregioncross.h
//
#include "fsregioncrosstestdummies.h"
////#include "fsregioncrosstest.h"

#include "fsregioncross.h"
#include "llviewerobjectdummy.h"

//
//  Dummy FrameTimer
//
namespace FrameTimer {
    static F64 mFrameTimer;
    F64 getElapsedSeconds()
    {   return(mFrameTimer);    
    }
};

//  
//  Dummy LLViewerObject
//

//  Parse data line from logging info
void LLViewerObject::parseline(const std::string& s)
{
}

//
//  Dummy quaternion math
//
LLVector3 operator*(const LLVector3 &a, const LLQuaternion &rot)	// Rotates a by rot
{
}







void test() 
{
    LLViewerObject* obj = new LLViewerObject();                     // new viewer object
    obj->mExtrap.changedlink(*obj);                                 // do a link change
    obj->mExtrap.update(*obj);                                      // update it 
    F32 lim = obj->mExtrap.getextraptimelimit();                    // get current time limit
    printf("Limit: %3.2f\n", lim);                                  // print limit
    delete(obj);                                                    // release object            
}

int main(int argc, char** argv)
{
    test();
}
