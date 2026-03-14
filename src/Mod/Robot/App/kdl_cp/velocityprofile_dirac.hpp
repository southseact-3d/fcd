#ifndef MOTIONPROFILE_DIRAC_H
#define MOTIONPROFILE_DIRAC_H

#include "velocityprofile.hpp"


namespace KDL {
	/**
	 * A Dirac VelocityProfile generates an infinite velocity
	 * so that the position jumps from A to B in an infinite short time.
	 * In practice, this means that the maximum values are ignored and
	 * for any t : Vel(t) == 0 and Acc(t) == 0.
	 * Further Pos( -0 ) = pos1 and Pos( +0 ) = pos2.
	 *
	 * However, if a duration is given, it will create an unbound
	 * rectangular velocity profile for that duration, otherwise,
	 * Duration() == 0;
	 * @ingroup Motion
	 */
	class VelocityProfile_Dirac : public VelocityProfile
	{
		double p1,p2,t;
	public:
		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(double pos1,double pos2,double duration);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
		virtual void Write(std::ostream& os) const;
		virtual VelocityProfile* Clone() const {
			VelocityProfile_Dirac* res =  new VelocityProfile_Dirac();
			res->SetProfileDuration( p1, p2, t );
			return res;
		}

		virtual ~VelocityProfile_Dirac() {}
	};

}


#endif
