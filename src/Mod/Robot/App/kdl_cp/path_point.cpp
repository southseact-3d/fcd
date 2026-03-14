#include "path_point.hpp"

namespace KDL {

Path_Point::Path_Point(const Frame& startpos)
    :F_base_start(startpos)
   {
   }

double Path_Point::LengthToS(double length) {
	return length;
}
double Path_Point::PathLength(){
	return 0;
}
Frame Path_Point::Pos(double /*s*/) const  {
	return F_base_start;
}

Twist Path_Point::Vel(double /*s*/,double /*sd*/) const  {
	return Twist::Zero();
}

Twist Path_Point::Acc(double /*s*/,double /*sd*/,double /*sdd*/) const  {
	return Twist::Zero();
}

Path_Point::~Path_Point() {
}

Path* Path_Point::Clone() {
	return new Path_Point( F_base_start	);
}

void Path_Point::Write(std::ostream& os)  {
	os << "POINT[ "<< F_base_start  << "]"  << std::endl;
}


}

