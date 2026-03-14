#include "path_cyclic_closed.hpp"
#include "utilities/error.h"

namespace KDL {

Path_Cyclic_Closed::Path_Cyclic_Closed(Path* _geom,int _times, bool _aggregate):
times(_times),geom(_geom), aggregate(_aggregate) {}

double Path_Cyclic_Closed::LengthToS(double /*length*/) {
	throw Error_MotionPlanning_Not_Applicable();
	return 0;
}

double Path_Cyclic_Closed::PathLength(){
	return geom->PathLength()*times;
}

Frame Path_Cyclic_Closed::Pos(double s) const  {
	return geom->Pos( fmod(s,geom->PathLength()) );
}

Twist Path_Cyclic_Closed::Vel(double s,double sd) const  {
	return geom->Vel( fmod(s,geom->PathLength()),sd );
}

Twist Path_Cyclic_Closed::Acc(double s,double sd,double sdd) const  {
	return geom->Acc( fmod(s,geom->PathLength()),sd,sdd );
}


Path_Cyclic_Closed::~Path_Cyclic_Closed() {
    if (aggregate)
        delete geom;
}

Path* Path_Cyclic_Closed::Clone() {
	return new Path_Cyclic_Closed(geom->Clone(),times, aggregate);
}

void Path_Cyclic_Closed::Write(std::ostream& os)  {
	os << "CYCLIC_CLOSED[ ";
	os << "  ";geom->Write(os);os << std::endl;
	os << "  " << times << std::endl;
	os << "]"  << std::endl;
}

}

