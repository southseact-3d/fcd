#ifndef MOTIONPROFILE_RECT_H
#define MOTIONPROFILE_RECT_H

#include "velocityprofile.hpp"


namespace KDL {
	/**
	 * A rectangular VelocityProfile generates a constant velocity
	 * for moving from A to B.
	 * @ingroup Motion
	 */
	class VelocityProfile_Rectangular : public VelocityProfile
		// Defines a rectangular velocityprofile.
		// (i.e. constant velocity)
	{
		double d,p,v;
	public:
		double maxvel;

		VelocityProfile_Rectangular(double _maxvel=0):
		  maxvel(_maxvel) {}
		// constructs motion profile class with <maxvel> as parameter of the
		// trajectory.

		void SetMax( double _maxvel );
		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(
			double pos1,double pos2,double duration);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
		virtual void Write(std::ostream& os) const;
		virtual VelocityProfile* Clone() const{
			VelocityProfile_Rectangular* res =  new VelocityProfile_Rectangular(maxvel);
			res->SetProfileDuration( p, p+v*d, d );
			return res;
		}
		// returns copy of current VelocityProfile object. (virtual constructor)
		virtual ~VelocityProfile_Rectangular() {}
	};

}


#endif
