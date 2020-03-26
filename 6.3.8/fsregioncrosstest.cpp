//
//  Unit test for fsregioncross.h
//
//
//  Dummy types
//
typedef float F32;
typedef bool BOOL;

#include "fsregioncross.h"

class LLViewerObject 
{
public:
    RegionCrossExtrapolate mExtrap;                                 // extrapolatormExt
};



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
