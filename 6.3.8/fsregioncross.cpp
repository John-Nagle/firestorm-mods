/** 
 * @file fsregioncross.cpp
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
 
#include "fsregioncrosstestdummies.h"                           // ***TEMP***
#include "fsregioncross.h"
#include "llviewerobjectdummy.h"                                // ***TEMP***

//
//  Improved region crossing prediction.
//
//
//  update -- called for each object update message to "vehicle" objects.
//
//  This is called for any object with an avatar sitting on it.
//  If the object never moves, it's not a vehicle and we can skip this.
//  If it has moved, we treat it as a vehicle.
//
void RegionCrossExtrapolateImpl::update()
{      
    printf("Update called.\n");                                 // ***TEMP***
    if (!mMoved)                                                // if not seen to move
    {   LLVector3 rawvel = mOwner.getVelocity();                // velocity in world coords
        if (rawvel.mV[VX] != 0.0 || rawvel.mV[VY] != 0.0 || rawvel.mV[VZ] != 0) // check for nonzero velocity
        {   mMoved = true; }                                    // moved, has seated avatar, thus is vehicle
        else
        {   return; }                                           // sitting on stationary object, skip this
    }
    //  Moving seat - do the extrapolation calculations
    F64 dt = 1.0/45.0;                                          // dt used on first value - one physics frame on server  
    F64 now = FrameTimer::getElapsedSeconds();                  // timestamp
    if (mPreviousUpdateTime != 0.0)
    {   dt = now - mPreviousUpdateTime;                         // change since last update
        //  ***SHOULD ADJUST FOR PING TIME AND TIME DILATION***
        //  ***IS THERE SOME BETTER WAY TO GET THE TIME OF AN OBJECT UPDATE?***
    }
    mPreviousUpdateTime = now;
    LLQuaternion rot = mOwner.getRotationRegion();              // transform in global coords
    const LLQuaternion& inverserot = rot.conjugate();           // transform global to local
    LLVector3 vel = mOwner.getVelocity()*inverserot;            // velocity in object coords
    LLVector3 angvel = mOwner.getAngularVelocity()*inverserot;  // angular velocity in object coords
    mFilteredVel.update(vel,dt);                                // accum into filter in object coords
    mFilteredAngVel.update(angvel,dt);                          // accum into filter 
    printf("dt: %6.3f     vel: <%4.3f,%4.3f,%4.3f>     filteredvel: <%4.3f,%4.3f,%4.3f>\n", dt,vel.mV[VX],vel.mV[VY],vel.mV[VZ], 
        mFilteredVel.get().mV[VX], mFilteredVel.get().mV[VY],mFilteredVel.get().mV[VZ]); // ***TEMP***
    printf("dt: %6.3f  angvel: <%4.3f,%4.3f,%4.3f>  filteredangvel: <%4.3f,%4.3f,%4.3f>\n", dt,angvel.mV[VX],angvel.mV[VY],angvel.mV[VZ], 
        mFilteredAngVel.get().mV[VX], mFilteredAngVel.get().mV[VY],mFilteredAngVel.get().mV[VZ]); // ***TEMP***
}
//
//  dividesafe -- floating divide with divide by zero check
//
//  Returns infinity for a divide by near zero.
//
static inline F32 dividesafe(F32 num, F32 denom)
{    return((denom > FP_MAG_THRESHOLD                           // avoid divide by zero
         || denom < -FP_MAG_THRESHOLD)        
        ? (num / denom)
        : std::numeric_limits<F32>::infinity());                // return infinity if zero divide
}
//
//  getextraptimelimit -- don't extrapolate further ahead than this during a region crossing.
//
//  Returns seconds of extrapolation that will probably stay within set limits of error.
//
F32 RegionCrossExtrapolateImpl::getextraptimelimit() const 
{
    //  Time limit is max allowed error / error. Returns worst case (smallest) of vel and angular vel limits.
    LLQuaternion rot = mOwner.getRotationRegion();              // transform in global coords
    const LLQuaternion& inverserot = rot.conjugate();           // transform global to local
    printf("velerr: %4.3f  allowed: %4.3f   angvelerr: %4.3f  allowed: %4.3f\n",               // ***TEMP***
        (mOwner.getVelocity()*inverserot - mFilteredVel.get()).length(),
        gRegionCrossExtrapolateControl.mVelError,
        (mOwner.getAngularVelocity()*inverserot - mFilteredAngVel.get()).length(),
        gRegionCrossExtrapolateControl.mAngVelError);
    return(std::min(
        dividesafe(gRegionCrossExtrapolateControl.mVelError,
            ((mOwner.getVelocity()*inverserot - mFilteredVel.get()).length())),
        dividesafe(gRegionCrossExtrapolateControl.mAngVelError,
            ((mOwner.getAngularVelocity()*inverserot - mFilteredAngVel.get()).length()))));
}  

//
//  ifsaton -- True if object is being sat upon.
//
//  Potential vehicle.
//
BOOL RegionCrossExtrapolate::ifsaton(const LLViewerObject& vo)  // true if root object and being sat on
{   if (!vo.isRoot()) { return(false); }                        // not root, cannot be sat upon
    for (auto iter = vo.mChildList.begin();                     // check for avatar as child of root
		 iter != vo.mChildList.end(); iter++)
	{
		LLViewerObject* child = *iter;                          // get child 
		if (child->isAvatar()) { return(true); }                // avatar as child, we have a sitter
	}
    return(false);                                              // no avatar children, not sat on
}

//
//  LowPassFilter  -- the low pass filter for smoothing velocities.
//
//  Works on vectors.
//
//  Filter function is scaled by 1- 1/((1+(1/filterval))^t)
//  This is so samples which represent more time are weighted more.
//
void LowPassFilter::update(const LLVector3& val, F32 secs)      // add new value into filter
{
    if (!mInitialized)                                          // if not initialized yet
    {   mFiltered = val;                                        // just use new value
        mInitialized = true;
        return;
    }
    F32 filtermult = 1.0;                                       // no filtering if zero filter time
    if (gRegionCrossExtrapolateControl.mFilterTime > 0.001)     // avoid divide by zero
    {   filtermult = 1.0 - 1.0/pow(1.0+1.0/gRegionCrossExtrapolateControl.mFilterTime,secs);  }        // filter scale factor
    printf("Filter mult: %6.3f\n",filtermult);                  // ***TEMP***
    mFiltered = val * filtermult + mFiltered*(1.0-filtermult);  // low pass filter
}
//
//  Control singleton
//
RegionCrossExtrapolateControl gRegionCrossExtrapolateControl;



