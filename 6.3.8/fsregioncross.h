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

class LLViewerObject;                                               // forward declaration

class RegionCrossExtrapolateImpl {                                  // Implementation of region cross extrapolation control
private:
    //  ***MORE***

public:
    RegionCrossExtrapolateImpl()                                    // constructor
    {
        printf("RegionCrossExtrapolateImpl initialized.\n");        // ***TEMP***
    }
    
    ~RegionCrossExtrapolateImpl()                                   // destructor
    {
        printf("RegionCrossExtrapolateImpl deleted.\n");            // ***TEMP***
    }

    void update(const LLViewerObject& vo)
    {
        printf("Update called.\n");                                 // ***TEMP***
    }
  
    F32 getextraptimelimit() const 
    {
        return(1.0);                                                // ***TEMP***
    }  
    
};

//
//  RegionCrossExtrapolate -- calculate safe limit on how long to extrapolate after a region crossing
//
//  Member object of llVieweObject. For vehicles, a RegionCrossExtrapolateImpl is allocated to do the real work.
//  Call "update" for each new object update.
//  Call "changedlink" for any object update which changes parenting.
//  Get the extrapolation limit time with getextraptimelimit.
//
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
    {   if (mImpl.get()) { mImpl->update(vo); }                     // update extrapolator if present
    }
    
    void changedlink(const LLViewerObject& vo)                      // parent or child changed, check if extrapolation object needed
    {
        if (isvehicle(vo))                                          // if this object is now the root of a vehicle with an avatar
        {   if (!mImpl.get())                                       // if no extrapolation implementor
            {   mImpl.reset(new RegionCrossExtrapolateImpl()); }    // add an extrapolator       
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
