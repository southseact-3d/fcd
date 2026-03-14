#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "frames.hpp"
#include "frames_io.hpp"
#include "path.hpp"
#include "velocityprofile.hpp"



namespace KDL {




	/**
	 * An abstract class that implements
	 * a trajectory contains a cartesian space trajectory and an underlying
	 * velocity profile.
	  * @ingroup Motion
	 */
	class Trajectory
	{
	public:
        virtual Path*      GetPath() = 0;
        // The underlying Path - FreeCAD change

        virtual VelocityProfile* GetProfile() = 0;
        // The velocity profile - FreeCAD change

		virtual double Duration() const = 0;
		// The duration of the trajectory

		virtual Frame Pos(double time) const = 0;
		// Position of the trajectory at <time>.

		virtual Twist Vel(double time) const = 0;
		// The velocity of the trajectory at <time>.
		virtual Twist Acc(double time) const = 0;
		// The acceleration of the trajectory at <time>.

		virtual Trajectory* Clone() const = 0;
		virtual void Write(std::ostream& os) const = 0;
		static Trajectory* Read(std::istream& is);
		virtual ~Trajectory() {}
		// note : you cannot declare this destructor abstract
		// it is always called by the descendant's destructor !
	};



}


#endif
