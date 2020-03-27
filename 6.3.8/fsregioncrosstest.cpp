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

LLVector3		operator*(const LLVector3 &a, const LLQuaternion &rot)
{
    F32 rw = - rot.mQ[VX] * a.mV[VX] - rot.mQ[VY] * a.mV[VY] - rot.mQ[VZ] * a.mV[VZ];
    F32 rx =   rot.mQ[VW] * a.mV[VX] + rot.mQ[VY] * a.mV[VZ] - rot.mQ[VZ] * a.mV[VY];
    F32 ry =   rot.mQ[VW] * a.mV[VY] + rot.mQ[VZ] * a.mV[VX] - rot.mQ[VX] * a.mV[VZ];
    F32 rz =   rot.mQ[VW] * a.mV[VZ] + rot.mQ[VX] * a.mV[VY] - rot.mQ[VY] * a.mV[VX];

    F32 nx = - rw * rot.mQ[VX] +  rx * rot.mQ[VW] - ry * rot.mQ[VZ] + rz * rot.mQ[VY];
    F32 ny = - rw * rot.mQ[VY] +  ry * rot.mQ[VW] - rz * rot.mQ[VX] + rx * rot.mQ[VZ];
    F32 nz = - rw * rot.mQ[VZ] +  rz * rot.mQ[VW] - rx * rot.mQ[VY] + ry * rot.mQ[VX];

    return LLVector3(nx, ny, nz);
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
