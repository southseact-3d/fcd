#ifndef KDL_MOTION_PATHCIRCLE_H
#define KDL_MOTION_PATHCIRCLE_H

#include "path.hpp"
#include "rotational_interpolation.hpp"


namespace KDL {


	/**
	 * A circular Path with 'open ends'. Path_Arc would
	 * have been a better name though.
	 * @ingroup Motion
	 */
class Path_Circle : public Path
	{

		// Orientatie gedeelte
		RotationalInterpolation* orient;

		// Circular gedeelte
		double radius;
		Frame  F_base_center;

		// equivalent radius
		double eqradius;

		// verdeling baanlengte over pos/rot
		double pathlength;
		double scalelin;
		double scalerot;

		bool aggregate;

	public:

		/**
		 *
		 * CAN THROW Error_MotionPlanning_Circle_ToSmall
		 * CAN THROW Error_MotionPlanning_Circle_No_Plane
		 */
		Path_Circle(const Frame& F_base_start,const Vector& V_base_center,
			const Vector& V_base_p,
			const Rotation& R_base_end,
			double alpha,
			RotationalInterpolation* otraj,
			double eqradius,
			bool _aggregate=true);

		double LengthToS(double length);

		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;
		virtual Path* Clone();
		virtual void Write(std::ostream& os);

		/**
		 * gets an identifier indicating the type of this Path object
		 */
		virtual IdentifierType getIdentifier() const {
			return ID_CIRCLE;
		}

		virtual ~Path_Circle();
	};


}


#endif
