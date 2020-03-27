/** 
 * @file fsregioncross.h
 * @brief Improvements to region crossing display
 * @author nagle@animats.com
 *
 * $LicenseInfo:firstyear=2013&license=viewerlgpl$
 * Phoenix Firestorm Viewer Source Code
 * Copyright (C) 2020 Animats
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * The Phoenix Firestorm Project, Inc., 1831 Oakwood Drive, Fairmont, Minnesota 56031-3225 USA
 * http://www.firestormviewer.org
 */

#ifndef FS_FSREGIONCROSS_H
#define FS_FSREGIONCROSS_H

//
//  Improved region crossing support.
//
#include <memory>
#include <limits>
#include "fsregioncrosstestdummies.h"  // ***TEMP***

class LLViewerObject;                                               // forward declaration

//
//  LowPassFilter -- a simple Kalman low-pass filter
//
class LowPassFilter 
{
private:
    LLVector3 mFiltered;                                            // filtered value
    F32 mFilterConstant;                                            // 0 to 1.0, weight of new values
public:
    void update(const LLVector3 val, F32 dt)
    {
        //  ***MORE***
    }
    const LLVector3 get() const 
    {
        LLVector3 dummy;
        return(dummy);                                              // ***TEMP***
    }
    
    void clear() 
    {
    }
};


class RegionCrossExtrapolateImpl :                              // Implementation of region cross extrapolation control 
    mPreviousUpdateTime(0.0)                                    // no previous time yet
{
private:
    const LLViewerObject& mOwner;                                   // ref to owning object
    F64 mPreviousUpdateTime;                                        // previous update time
    LowPassFilter mFilteredVel;                                     // filtered velocity
    LowPassFilter mFilteredAngVel;                                  // filtered angular velocity

public:
    RegionCrossExtrapolateImpl(const LLViewerObject& vo) :          // constructor
    mOwner(vo),                                                     // back ref to owner
    mPreviousUpdateTime(0)                                          // time of last update
    {
        printf("RegionCrossExtrapolateImpl initialized.\n");        // ***TEMP***
        
    }
    
    ~RegionCrossExtrapolateImpl()                                   // destructor
    {
        printf("RegionCrossExtrapolateImpl deleted.\n");            // ***TEMP***
    }

    void update();                                                  // update on object update message
  
    F32 getextraptimelimit() const 
    {
        return(1.0);                                                // ***TEMP***
    }  
    
};

//
//  RegionCrossExtrapolate -- calculate safe limit on how long to extrapolate after a region crossing
//
//  Member object of llViewerObject. For vehicles, a RegionCrossExtrapolateImpl is allocated to do the real work.
//  Call "update" for each new object update.
//  Call "changedlink" for any object update which changes parenting.
//  Get the extrapolation limit time with getextraptimelimit.
//
class LLViewerObject;                                               // forward
class RegionCrossExtrapolate {
private:
    std::unique_ptr<RegionCrossExtrapolateImpl> mImpl;              // pointer to region cross extrapolator, if present
    
protected:
    BOOL isvehicle(const LLViewerObject& vo)                        // true if vehicle
    {   
        return(true);                                               // ***TEMP***
    }
    
public:
    void update(const LLViewerObject& vo)                           // new object update message received
    {   if (mImpl.get()) { mImpl->update(); }                       // update extrapolator if present
    }
    
    void changedlink(const LLViewerObject& vo)                      // parent or child changed, check if extrapolation object needed
    {
        if (isvehicle(vo))                                          // if this object is now the root of a vehicle with an avatar
        {   if (!mImpl.get())                                       // if no extrapolation implementor
            {   mImpl.reset(new RegionCrossExtrapolateImpl(vo)); }  // add an extrapolator       
        } else {                                                    // not a vehicle
            if (mImpl.get())
            {   mImpl.reset(); }                                    // no longer needed                           
        }
    }
    
    F32 getextraptimelimit() const                                  // get extrapolation time limit
    {   if (mImpl.get()) { return(mImpl->getextraptimelimit()); }   // get extrapolation time limit if vehicle
        return(std::numeric_limits<F32>::infinity());               // no limit if not a vehicle
    }
};

#endif // FS_REGIONCROSS_H
