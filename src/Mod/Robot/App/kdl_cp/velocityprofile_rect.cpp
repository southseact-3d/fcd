#include "utilities/error.h"
#include "velocityprofile_rect.hpp"

namespace KDL {


void VelocityProfile_Rectangular::SetProfile(
	double pos1,
	double pos2
	)
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
    if ( diff != 0 )
        {
            v    = (diff>0)?maxvel:-maxvel;
            p    = pos1;               // start pos
            d    = diff/v;
        }
    else
        {
            v = 0;
            p = pos1;
            d = 0;
        }
}

    void VelocityProfile_Rectangular::SetMax( double vMax )
    {
        maxvel = vMax;
    }


void VelocityProfile_Rectangular::
	SetProfileDuration(double pos1,double pos2,double duration)
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
    if ( diff != 0 )
        {
            v    = diff/duration;
            if (v > maxvel || duration==0 ) // safety.
                v=maxvel;
            p    = pos1;               // start pos
            d    = diff/v;
        }
    else
        {
            v    = 0;
            p    = pos1;
            d    = duration;
        }
}

double VelocityProfile_Rectangular::Duration() const {
	return d;
}

double VelocityProfile_Rectangular::Pos(double time) const {
    if (time < 0) {
        return p;
    } else if (time>d) {
        return v*d+p;
    } else {
        return v*time+p;
    }
}

double VelocityProfile_Rectangular::Vel(double time) const {
    if (time < 0) {
        return 0;
    } else if (time>d) {
        return 0;
    } else {
        return v;
    }
}

double VelocityProfile_Rectangular::Acc(double /*time*/) const {
	throw Error_MotionPlanning_Incompatible();
}


void VelocityProfile_Rectangular::Write(std::ostream& os) const {
	os << "CONSTVEL[" << maxvel << "]";
}


}

