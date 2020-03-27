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
#include "fsregioncrosstestdummies.h"
#include "fsregioncross.h"
#include "llviewerobjectdummy.h"
////#include "fsregioncrosstest.h"

//
//  Improved region crossing support.
//
void RegionCrossExtrapolateImpl::update()
{      
    printf("Update called.\n");                                 // ***TEMP***
    F64 dt = 1.0/45.0;    
    F64 now = FrameTimer::getElapsedSeconds();                  // timestamp
    if (mPreviousUpdateTime != 0.0)
    {   dt = now - mPreviousUpdateTime;                         // change since last update
        //  ***SHOULD ADJUST FOR PING TIME AND TIME DILATION***
        //  ***IS THERE SOME BETTER WAY TO GET THE TIME OF AN OBJECT UPDATE?***
    }
    mPreviousUpdateTime = now;
    LLQuaternion rot = mOwner.getRotationRegion(); // transform global to local
    const LLQuaternion inverserot = rot.conjugate();
    LLVector3 vel = mOwner.getVelocity();                       // velocity in world coords
    vel = vel * inverserot;                                     // velocity in object coords
    mFilteredVel.update(vel,dt);                                // accum into filter
    LLVector3 angvel = mOwner.getAngularVelocity();             // angular velocity in world coords
    angvel = angvel * inverserot;                               // angular velocity in object coords
    mFilteredAngVel.update(angvel, dt);                         // accum into filter
        
}

