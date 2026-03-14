#ifndef KDL_MOTION_PATH_POINT_H
#define KDL_MOTION_PATH_POINT_H

#include "path.hpp"
#include "rotational_interpolation.hpp"


namespace KDL {



/**
 * A Path consisting only of a point in space.
 * @ingroup Motion
 */
class Path_Point : public Path
	{
		Frame F_base_start;
	public:
		/**
		 * Constructs a Point Path
		 */
		Path_Point(const Frame& F_base_start);
		double LengthToS(double length);
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const ;
		virtual Twist Acc(double s,double sd,double sdd) const;
		virtual void Write(std::ostream& os);
		virtual Path* Clone();

		/**
		 * gets an identifier indicating the type of this Path object
		 */
		virtual IdentifierType getIdentifier() const {
			return ID_POINT;
		}
		virtual ~Path_Point();
	};

}


#endif
