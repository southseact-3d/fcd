#ifndef KDL_MOTION_PATH_H
#define KDL_MOTION_PATH_H

#include "frames.hpp"

#include <vector>

#include "frames_io.hpp"

namespace KDL {

/**
 * The specification of the path of a trajectory.
 */
class Path
	{
	public:
		enum IdentifierType {
			ID_LINE=1,
			ID_CIRCLE=2,
			ID_COMPOSITE=3,
			ID_ROUNDED_COMPOSITE=4,
			ID_POINT=5,
			ID_CYCLIC_CLOSED=6
		};
		/**
		 * LengthToS() converts a physical length along the trajectory
		 * to the parameter s used in Pos, Vel and Acc.  This is used because
		 * in cases with large rotations the parameter s does NOT correspond to
		 * the lineair length along the trajectory.
		 * User should be sure that the lineair distance travelled by this
		 * path object is NOT zero, when using this method !
		 * (e.g. the case of only rotational change)
		 * throws Error_MotionPlanning_Not_Applicable if used on composed
		 * path objects.
		 * @ingroup Motion
		 */
		virtual double LengthToS(double length)  = 0;

		/**
		 * Returns the total path length of the trajectory
		 * (has dimension LENGTH)
		 * This is not always a physical length , ie when dealing with rotations
		 * that are dominant.
		 */
		virtual double PathLength() = 0;

		/**
		 * Returns the Frame at the current path length s
		 */
		virtual Frame Pos(double s) const = 0;

		/**
		 * Returns the velocity twist at path length s theta and with
		 * derivative of s == sd
		 */
		virtual Twist Vel(double s,double sd) const  = 0;

		/**
		 * Returns the acceleration twist at path length s and with
		 * derivative of s == sd, and 2nd derivative of s == sdd
		 */
		virtual Twist Acc(double s,double sd,double sdd) const  = 0;

		/**
		 * Writes one of the derived objects to the stream
		 */
		virtual void Write(std::ostream& os)  = 0;

		/**
		 * Reads one of the derived objects from the stream and returns a pointer
		 * (factory method)
		 */
		static Path* Read(std::istream& is);

		/**
		 * Virtual constructor, constructing by copying,
		 * Returns a deep copy of this Path Object
		 */
		virtual Path* Clone() = 0;

		/**
		 * gets an identifier indicating the type of this Path object
		 */
		virtual IdentifierType getIdentifier() const=0;

		virtual ~Path() {}
	};

}


#endif
