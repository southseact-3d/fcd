#ifndef KDL_MOTION_PATH_CYCLIC_CLOSED_H
#define KDL_MOTION_PATH_CYCLIC_CLOSED_H

#include "frames.hpp"
#include "frames_io.hpp"
#include "path.hpp"
#include <vector>


namespace KDL {

	 /**
	  * A Path representing a closed circular movement,
	  * which is traversed a number of times.
	  * @ingroup Motion
	  */
	 class Path_Cyclic_Closed : public Path
	{
		int times;
		Path* geom;
		bool aggregate;
	public:
		Path_Cyclic_Closed(Path* _geom,int _times, bool _aggregate=true);
		virtual double LengthToS(double length);
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;

		virtual void Write(std::ostream& os);
		static Path* Read(std::istream& is);
		virtual Path* Clone();
		/**
		 * gets an identifier indicating the type of this Path object
		 */
		virtual IdentifierType getIdentifier() const {
			return ID_CYCLIC_CLOSED;
		}
		virtual ~Path_Cyclic_Closed();
	};



}


#endif
