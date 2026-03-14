#ifndef KDL_ROTATIONALINTERPOLATION_H
#define KDL_ROTATIONALINTERPOLATION_H

#include "frames.hpp"
#include "frames_io.hpp"

namespace KDL {

/**
 * RotationalInterpolation specifies the rotational part of a geometric trajectory
 * -   The different derived objects specify different methods for interpolating
 *    rotations.
 * - SetStartEnd should be called before
 *   using the other methods
 * - The start and end position do NOT belong to the persistent state !  The owner of this
 *   object is responsible for setting these each time
 * @ingroup Motion
 */
class RotationalInterpolation
	{
	public:
		/**
		 * Set the start and end rotational_interpolation
		 */
		virtual void SetStartEnd(Rotation start,Rotation end) = 0;

		/**
		 * - Returns the angle value to move from start to end.
		 * This should have units radians,
		 * - With Single Axis interp corresponds to the angle rotation
		 * - With Three Axis interp corresponds to the slowest of the three
		 * rotations.
		 */
		virtual double Angle() = 0;

		/**
		 * Returns the rotation matrix at angle theta
		 */
		virtual Rotation Pos(double theta) const = 0;

		/**
		 * Returns the rotational velocity at angle theta and with
		 * derivative of theta == thetad
		 */
		virtual Vector Vel(double theta,double thetad) const = 0;

		/**
		 * Returns the rotational acceleration at angle theta and with
		 * derivative of theta == thetad, and 2nd derivative of theta == thdd
		 */
		virtual Vector Acc(double theta,double thetad,double thetadd) const = 0;

		/**
		 * Writes one of the derived objects to the stream
		 */
		virtual void Write(std::ostream& os) const = 0;

		/**
		 * Reads one of the derived objects from the stream and returns a pointer
		 * (factory method)
		 */
		static RotationalInterpolation* Read(std::istream& is);

		/**
		 * virtual constructor,  construction by copying.
		 */
		virtual RotationalInterpolation* Clone() const = 0;

		virtual ~RotationalInterpolation() {}
	};

}


#endif
