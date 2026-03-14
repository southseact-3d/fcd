#include "path_composite.hpp"
#include "utilities/error.h"
#include <memory>

namespace KDL {

// s should be in allowable limits, this is not checked
// simple linear search : TODO : make it binary search
// uses cached_... variables
// returns the relative path length within the segment
// you probably want to use the cached_index variable
double Path_Composite::Lookup(double s) const
{
	assert(s>=-1e-12);
	assert(s<=pathlength+1e-12);
	if ( (cached_starts <=s) && ( s <= cached_ends) ) {
		return s - cached_starts;
	}
	double previous_s=0;
	for (unsigned int i=0;i<dv.size();++i) {
		if ((s <= dv[i])||(i == (dv.size()-1) )) {
			cached_index = i;
			cached_starts = previous_s;
			cached_ends   = dv[i];
			return s - previous_s;
		}
		previous_s = dv[i];
	}
	return 0;
}

Path_Composite::Path_Composite() {
	pathlength    = 0;
	cached_starts = 0;
	cached_ends   = 0;
	cached_index  = 0;
}

void Path_Composite::Add(Path* geom, bool aggregate ) {
	pathlength += geom->PathLength();
	dv.insert(dv.end(),pathlength);
	gv.insert( gv.end(),std::make_pair(geom,aggregate) );
}

double Path_Composite::LengthToS(double /*length*/) {
	throw Error_MotionPlanning_Not_Applicable();
}

double Path_Composite::PathLength() {
	return pathlength;
}


Frame Path_Composite::Pos(double s) const {
	s = Lookup(s);
	return gv[cached_index].first->Pos(s);
}

Twist Path_Composite::Vel(double s,double sd) const {
	s = Lookup(s);
	return gv[cached_index].first->Vel(s,sd);
}

Twist Path_Composite::Acc(double s,double sd,double sdd) const {
	s = Lookup(s);
	return gv[cached_index].first->Acc(s,sd,sdd);
}

Path* Path_Composite::Clone()  {
	std::unique_ptr<Path_Composite> comp( new Path_Composite() );
	for (unsigned int i = 0; i < dv.size(); ++i) {
		comp->Add(gv[i].first->Clone(), gv[i].second);
	}
	return comp.release();
}

void Path_Composite::Write(std::ostream& os)  {
	os << "COMPOSITE[ " << std::endl;
	os << "   " << dv.size() << std::endl;
	for (unsigned int i=0;i<dv.size();i++) {
		gv[i].first->Write(os);
	}
	os << "]" << std::endl;
}

int Path_Composite::GetNrOfSegments() {
	return dv.size();
}

Path* Path_Composite::GetSegment(int i) {
	assert(i>=0);
	assert(i<static_cast<int>(dv.size()));
	return gv[i].first;
}

double Path_Composite::GetLengthToEndOfSegment(int i) {
	assert(i>=0);
	assert(i<static_cast<int>(dv.size()));
	return dv[i];
}

void Path_Composite::GetCurrentSegmentLocation(double s, int& segment_number,
		double& inner_s)
{
	inner_s = Lookup(s);
	segment_number= cached_index;
}

Path_Composite::~Path_Composite() {
	PathVector::iterator it;
	for (it=gv.begin();it!=gv.end();++it) {
		if (it->second)
            delete it->first;
	}
}

} // namespace KDL
