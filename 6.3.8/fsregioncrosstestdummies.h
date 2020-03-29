#ifndef FSREGIONCROSSDUMMIES_H
#define FSREGIONCROSSDUMMIES_H
#include <math.h>
//
//  Dummy types normally defined within Firestorm/LL code
//
//  For unit test only. Not needed inside viewer.
//
typedef float F32;
typedef double F64;
typedef int U32;
typedef bool BOOL;

const int VX = 0;
const int VY = 1;
const int VZ = 2;
const int VW = 3;
const int VS = VW;  // huh?

class LLVector3 {                                                  // dummy of LL vector type
public:
    F32 mV[3]; 
    
    LLVector3(F32 nx, F32 ny, F32 nz)
    {   mV[VX] = nx; mV[VY] = ny; mV[VZ] = nz; }
    
    LLVector3() {}
    LLVector3 operator*(F32 a) const
    {   return(LLVector3(mV[VX]*a, mV[VY]*a, mV[VZ]*a)); }   
    
    LLVector3 operator+(const LLVector3 a) const
    {   return(LLVector3(mV[VX]+a.mV[VX], mV[VY]+a.mV[VY], mV[VZ]+a.mV[VZ])); }  
    
    LLVector3 operator-(const LLVector3 a) const
    {   return(LLVector3(mV[VX]-a.mV[VX], mV[VY]-a.mV[VY], mV[VZ]-a.mV[VZ])); }
    
    F32		length() const;			// Returns magnitude of LLVector3               
};

inline F32 LLVector3::length() const			// Returns magnitude of LLVector3
{
    return(sqrt(
        mV[VX]*mV[VX] + mV[VY]*mV[VY] + mV[VZ]*mV[VZ]));
}

static const U32 LENGTHOFQUAT = 4;


class LLQuaternion                                                  // dummy of LL quaternion type
{
public:
	F32 mQ[LENGTHOFQUAT];
	
	LLQuaternion(F32 nx, F32 ny, F32 nz, F32 nw)
    {   mQ[VX] = nx; mQ[VY] = ny; mQ[VZ] = nz; mQ[VW] = nw;}
    LLQuaternion() {}
    
    friend LLVector3 operator*(const LLVector3 &a, const LLQuaternion &rot);		// Rotates a by rot
    const LLQuaternion&	conjugate();		// transpose (same as conjugate) - in place
    const LLQuaternion&	set(const F32 *q);
    F32     normalize();
    
};
inline const LLQuaternion& LLQuaternion::conjugate()        // in-place conjugate/transpose
{
	mQ[VX] *= -1.f;
	mQ[VY] *= -1.f;
	mQ[VZ] *= -1.f;
	return (*this);
}

inline const LLQuaternion&	LLQuaternion::set(const F32 *q)
{
	mQ[VX] = q[VX];
	mQ[VY] = q[VY];
	mQ[VZ] = q[VZ];
	mQ[VS] = q[VW];
	normalize();
	return (*this);
}




const F32 FP_MAG_THRESHOLD = 0.0000001f;
const F32 ONE_PART_IN_A_MILLION = 0.000001f;




//  Normalize, from Firestorm.
inline F32	LLQuaternion::normalize()
{
	F32 mag = sqrtf(mQ[VX]*mQ[VX] + mQ[VY]*mQ[VY] + mQ[VZ]*mQ[VZ] + mQ[VS]*mQ[VS]);

	if (mag > FP_MAG_THRESHOLD)
	{
		// Floating point error can prevent some quaternions from achieving
		// exact unity length.  When trying to renormalize such quaternions we
		// can oscillate between multiple quantized states.  To prevent such
		// drifts we only renomalize if the length is far enough from unity.
		if (fabs(1.f - mag) > ONE_PART_IN_A_MILLION)
		{
			F32 oomag = 1.f/mag;
			mQ[VX] *= oomag;
			mQ[VY] *= oomag;
			mQ[VZ] *= oomag;
			mQ[VS] *= oomag;
		}
	}
	else
	{
		// we were given a very bad quaternion so we set it to identity
		mQ[VX] = 0.f;
		mQ[VY] = 0.f;
		mQ[VZ] = 0.f;
		mQ[VS] = 1.f;
	}

	return mag;
}

namespace FrameTimer {
    F64 getElapsedSeconds();      // forward
};

#endif // FSREGIONCROSSDUMMIES_H
