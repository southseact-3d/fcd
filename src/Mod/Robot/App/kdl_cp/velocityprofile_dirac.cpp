#include "utilities/error.h"
#include "velocityprofile_dirac.hpp"

namespace KDL {


    void VelocityProfile_Dirac::SetProfile(
                                           double pos1,
                                           double pos2
                                           )
    {
        p1 = pos1;
        p2 = pos2;
        t = 0;
    }

    void VelocityProfile_Dirac::
	SetProfileDuration(double pos1,double pos2,double duration)
    {
        SetProfile(pos1,pos2);
        t = duration;
    }

    double VelocityProfile_Dirac::Duration() const {
        return t;
    }

    double VelocityProfile_Dirac::Pos(double time) const {
        if ( t == 0 )
            return time == 0 ? p1 : p2;
        else
            return p1 + (( p2 - p1)/t)*time;
    }

    double VelocityProfile_Dirac::Vel(double time) const {
        if ( t == 0 )
            {
            throw Error_MotionPlanning_Incompatible();
            }
        else
            if ( 0 < time && time < t )
                return (p2-p1) / t;
        return 0;
    }

    double VelocityProfile_Dirac::Acc(double /*time*/) const {
        throw Error_MotionPlanning_Incompatible();
    }


    void VelocityProfile_Dirac::Write(std::ostream& os) const {
        os << "DIRACVEL[ ]";
    }



}

