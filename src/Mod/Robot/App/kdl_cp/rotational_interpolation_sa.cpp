#include "rotational_interpolation_sa.hpp"
#include "trajectory.hpp"

namespace KDL {


RotationalInterpolation_SingleAxis::RotationalInterpolation_SingleAxis()
	{}

void RotationalInterpolation_SingleAxis::SetStartEnd(Rotation start,Rotation end) {
	R_base_start = start;
	R_base_end   = end;
	Rotation R_start_end = R_base_start.Inverse()*R_base_end;
	angle = R_start_end.GetRotAngle(rot_start_end);
}

Rotation RotationalInterpolation_SingleAxis::Pos(double theta) const {
	return R_base_start* Rotation::Rot2(rot_start_end,theta);
}

Vector RotationalInterpolation_SingleAxis::Vel(double /*theta*/,double thetad) const {
	return R_base_start * ( rot_start_end*thetad );
}

Vector RotationalInterpolation_SingleAxis::Acc(double /*theta*/,double /*thetad*/,double thetadd) const {
	return R_base_start * ( rot_start_end* thetadd);
}

double RotationalInterpolation_SingleAxis::Angle() {
	return angle;
}

void RotationalInterpolation_SingleAxis::Write(std::ostream& os) const {
	os << "SingleAxis[] " << std::endl;
}

RotationalInterpolation_SingleAxis::~RotationalInterpolation_SingleAxis() {
}


RotationalInterpolation* RotationalInterpolation_SingleAxis::Clone() const {
	return new RotationalInterpolation_SingleAxis();
}

}

