#ifndef KDL_MOTION_VELOCITYPROFILE_TRAPHALF_H
#define KDL_MOTION_VELOCITYPROFILE_TRAPHALF_H

#include "velocityprofile.hpp"




namespace KDL {


	/**
	 * A 'Half' Trapezoidal VelocityProfile. A constructor flag
	 * indicates if the calculated profile should be starting
	 * or ending.
	 * @ingroup Motion
	 */
class VelocityProfile_TrapHalf : public VelocityProfile
	{
		// For "running" a motion profile :
		double a1,a2,a3; // coef. from ^0 -> ^2 of first part
		double b1,b2,b3; // of 2nd part
		double c1,c2,c3; // of 3th part
		double duration;
		double t1,t2;

		double startpos;
		double endpos;

		// Persistent state :
		double maxvel;
		double maxacc;
		bool   starting;

		void PlanProfile1(double v,double a);
		void PlanProfile2(double v,double a);
	public:

		/**
		 * \param _maxvel maximal velocity of the motion profile (positive)
		 * \param _maxacc maximal acceleration of the motion profile (positive)
		 * \param _starting this value is true when initial velocity is zero
		 *        and ending velocity is maxvel, is false for the reverse
		 */
		VelocityProfile_TrapHalf(double _maxvel=0,double _maxacc=0,bool _starting=true);

		void SetMax(double _maxvel,double _maxacc,bool _starting);

		/**
		 * Plans a 'Half' Trapezoidal VelocityProfile between pos1 and pos2.
		 * If the distance is too short between pos1 and pos2,
		 * only the acceleration phase is set and the max velocity is not reached.
		 *
		 * \param pos1 Starting position
		 * \param pos2 Ending position
		 *
		 * Can throw a Error_MotionPlanning_Not_Feasible
		 */
		virtual void SetProfile(double pos1,double pos2);

		/**
		 * Can be used to prolong the profile, there are two possible outcomes: in a first
		 * phase the acceleration is lowered as such that the end position and maximum velocity
		 * are reached at the given duration (newduration). In this case there is an acceleration part and a constant velocity part,
		 * when this reaches a minimum acceleration value at which the constant part disappears, the motion is stalled,
		 * in this case their is a non-motion part and an acceleration part.
		 *
		 *\param pos1 starting position
		 *\param pos2 ending position
		 *\param newduration the desired duration, if it is lower than the minimum duration, the minimum duration will be used instead of the given duration.
		 */
		virtual void SetProfileDuration(
			double pos1,double pos2,double newduration
		);

		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
		virtual void Write(std::ostream& os) const;
		virtual VelocityProfile* Clone() const;

		virtual ~VelocityProfile_TrapHalf();
	};



}


#endif
