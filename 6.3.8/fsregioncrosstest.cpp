//
//  Unit test for fsregioncross.h
//
#include "fsregioncrosstest.h"
#include "fsregioncross.h"



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
