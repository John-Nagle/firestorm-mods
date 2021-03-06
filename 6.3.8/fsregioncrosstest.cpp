//
//  Unit test for fsregioncross.h
//
#include <iostream>
#include <fstream>
#include "fsregioncrosstestdummies.h"

#include "fsregioncross.h"
#include "llviewerobjectdummy.h"

//
//  Dummy FrameTimer
//
namespace LLFrameTimer {
    static F64 mFrameTimer;
    F64 getElapsedSeconds()
    {   return(mFrameTimer);    
    }
};
//
//  Dummy debug controls
//
LLControlGroup gSavedSettings;

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

void test1() 
{
    LLViewerObject* obj = new LLViewerObject();                     // new viewer object
    obj->mExtrap.changedlink(*obj);                                 // do a link change
    obj->mExtrap.update(*obj);                                      // update it 
    F32 lim = obj->mExtrap.getextraptimelimit();                    // get current time limit
    printf("Limit: %3.2f\n", lim);                                  // print limit
    delete(obj);                                                    // release object            
}

//
//  dotestline -- do a test line
//
void dotestline(LLViewerObject& vo, float t, LLVector3 p, LLQuaternion r, LLVector3 v, LLVector3 av, const char* region)
{
    r.normalize();                                                  // normalize quaternion
    vo.mTime = t;
    vo.mPos = p;
    vo.mRot = r;
    vo.mVel = v;
    vo.mAngVel = av;
    LLFrameTimer::mFrameTimer = t;
    vo.mExtrap.update(vo);                                          // do the update
    F32 lim = vo.mExtrap.getextraptimelimit();                      // get current time limit
    printf("%6.2f (%s) Extrapolation limit: %3.2f secs.\n", t,region,lim);              // print limit 
}
//
//  dofile -- read file of test data.
//
//  Test data format is lines of the form:
//
//  t,px,py,pz,rx,ry,rz,rw,vx,vy,vz,avx,avy,avz,rg
//
//  t: time
//  p: position
//  r: rotation (quaternion)
//  v: velocity
//  av: angular velocity
//  rg: region name
//
void dofile(const char* filename, bool verbose)
{
    //  Set debug params
    const F32 defaultPositionErrLimit = 1.0;                     // (m) default position error limit
    const F32 defaultAngleErrLimit = 20.0;                       // (degrees) default angle error limit 
    const F32 defaultSmoothingTime = 2.0;                        // default smoothing time for predictor

    //  Create seat with avatar 
    LLViewerObject vo;                          // dummy viewer object
    LLViewerObject avi;                         // dummy avatar
    avi.mIsAvatar = true;
    vo.mChildList.push_back(&avi);              // avatar is child of vo
    avi.mParent = &vo;                          // backlink
    vo.mExtrap.changedlink(vo);                 // update for link change
    //  Open input file of logged test data
    std::cout << "Data from " << filename << std::endl;
    std::ifstream infile(filename);             // open input file
    if (!infile.is_open())
    {   std::cout << "Unable to open " << filename << std::endl; 
        return;
    }
    std::string line;
    while(!infile.eof())
    {   std::getline(infile,line);
        if (verbose)
        {   std::cout << line << std::endl; }   // list input
        if (line.length() == 0 || line[0] == '#') continue; // line to ignore
        //  Parseable line. Extract fields.
        int expectedcnt = 15;
        float t;
        LLVector3 p,v,av;
        LLQuaternion r;
        char region[61];
        int cnt = sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%60[^,]", 
            &t, 
            &p.mV[VX], &p.mV[VY], &p.mV[VZ],
            &r.mQ[VX], &r.mQ[VY], &r.mQ[VZ], &r.mQ[VW],
            &v.mV[VX], &v.mV[VY], &v.mV[VZ],
            &av.mV[VX], &av.mV[VY], &av.mV[VZ],
            region);
        if (cnt != expectedcnt) 
        {   std::cerr << line << std::endl << "Expected " << expectedcnt << " values, received " << cnt << std::endl;
            exit(1);
        }
        //
        //  Do test line
        //
        dotestline(vo, t, p, r, v, av, region);             // do a test line
    }
}

int main(int argc, char** argv)
{
    bool verbose = false;
    if (argc < 2)                                                   // if no args, do basic test
    {   test1(); return(0); }
    //  Parse args, process files
    for (int i=1; i<argc; i++)
    {   char* arg = argv[i];                                        // nth arg
        if (arg[0] == '-')                                          // if option
        {   switch (arg[1])
            {   case 'v':   verbose = true; break;                  // -v
                default: std::cerr << "Unknown option: " << arg << std::endl; return(1); 
            }
        } else {
            dofile(arg, verbose);                                   // do indicated file
        }
    }
}
