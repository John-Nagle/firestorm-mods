#ifndef FSREGIONCROSSDUMMIES_H
#define FSREGIONCROSSDUMMIES_H
#include <math.h>
//
//  Dummy types normally defined within Firestorm
//
typedef float F32;
typedef double F64;
typedef int U32;
typedef bool BOOL;

struct LLVector3 {                                                  // dummy
    F32 mV[3];   
};

static const U32 LENGTHOFQUAT = 4;
const int VX = 0;
const int VY = 1;
const int VZ = 2;
const int VW = 3;
const int VS = VW;  // huh?

class LLQuaternion
{
public:
	F32 mQ[LENGTHOFQUAT];
    
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
