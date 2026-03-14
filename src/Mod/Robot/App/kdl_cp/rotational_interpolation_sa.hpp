#ifndef KDL_ROTATIONALINTERPOLATION_SINGLEAXIS_H
#define KDL_ROTATIONALINTERPOLATION_SINGLEAXIS_H

#include "frames.hpp"
#include "frames_io.hpp"
#include "rotational_interpolation.hpp"


namespace KDL {


	 /**
	  * An interpolation algorithm which rotates a frame over the existing
	  * single rotation axis
	  * formed by start and end rotation. If more than one rotational axis
	  * exist, an arbitrary one will be chosen, therefore it is not recommended 
	  * to try to interpolate a 180 degrees rotation.
	  * @ingroup Motion
	  */
class RotationalInterpolation_SingleAxis: public RotationalInterpolation
	{
		Rotation R_base_start;
		Rotation R_base_end;
		Vector rot_start_end;
		double angle;
	public:
		RotationalInterpolation_SingleAxis();
		void SetStartEnd(Rotation start,Rotation end) override;
		double Angle() override;
		Rotation Pos(double th) const override;
		Vector Vel(double th,double thd) const override;
		Vector Acc(double th,double thd,double thdd)   const override;
		void Write(std::ostream& os) const override;
		RotationalInterpolation* Clone() const override;
		~RotationalInterpolation_SingleAxis() override;
	};

}


#endif
