#ifndef KDL_MOTION_TRAJECTORY_SEGMENT_H
#define KDL_MOTION_TRAJECTORY_SEGMENT_H

#include "frames.hpp"
#include "frames_io.hpp"
#include "trajectory.hpp"
#include "path.hpp"
#include "velocityprofile.hpp"


namespace KDL {


	/**
	 * Trajectory_Segment combines a VelocityProfile and a Path into a
	 * trajectory
	 * @ingroup Motion
	 */
	class Trajectory_Segment :  public Trajectory
	{
		VelocityProfile* motprof;
		Path*      geom;
		bool aggregate;
	public:
		/**
		 * This constructor assumes that \a geom and \<_motprof\> are initialised correctly.
		 */
		Trajectory_Segment(Path* _geom, VelocityProfile* _motprof, bool _aggregate=true);

		/**
		 * This constructor assumes that \a geom is initialised and \<_motprof\> needs to be
		 * set according to \a duration.
		 */
		Trajectory_Segment(Path* _geom, VelocityProfile* _motprof, double duration, bool _aggregate=true);

		virtual double Duration() const;
		// The duration of the trajectory

		virtual Frame Pos(double time) const;
		// Position of the trajectory at <time>.

		virtual Twist Vel(double time) const;
		// The velocity of the trajectory at <time>.
		virtual Twist Acc(double time) const;
		// The acceleration of the trajectory at <time>.

 		virtual Trajectory* Clone() const
			{
				if ( aggregate )
					return new Trajectory_Segment( geom->Clone(), motprof->Clone(), true );
				return new Trajectory_Segment( geom, motprof, false );
			}

		virtual void Write(std::ostream& os) const;

	    virtual Path* GetPath();

	    virtual VelocityProfile* GetProfile();


		virtual ~Trajectory_Segment();
	};



}


#endif
