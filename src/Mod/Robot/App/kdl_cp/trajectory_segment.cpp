#include "trajectory_segment.hpp"


namespace KDL {


Trajectory_Segment::Trajectory_Segment(Path* _geom, VelocityProfile* _motprof, bool _aggregate):
	motprof(_motprof),geom(_geom), aggregate(_aggregate)
{
    // assume everything is set or at least check if Duration() != 0
}

Trajectory_Segment::Trajectory_Segment(Path* _geom, VelocityProfile* _motprof, double _duration, bool _aggregate):
	motprof(_motprof),geom(_geom), aggregate(_aggregate)
{
    // the duration was specified so assume motprof not yet set.
    motprof->SetProfileDuration(0, geom->PathLength(), _duration);
}


double Trajectory_Segment::Duration() const
{
	return motprof->Duration();
}

Frame Trajectory_Segment::Pos(double time) const
{
	return geom->Pos(motprof->Pos(time));
}

Twist Trajectory_Segment::Vel(double time) const
{
	return geom->Vel(motprof->Pos(time),motprof->Vel(time));
}

Twist Trajectory_Segment::Acc(double time) const
{
	return geom->Acc(motprof->Pos(time),motprof->Vel(time),motprof->Acc(time));
}


void Trajectory_Segment::Write(std::ostream& os) const
{
	os << "SEGMENT[ " << std::endl;
	os << "  ";geom->Write(os);os << std::endl;
	os << "  ";motprof->Write(os);os << std::endl;
	os << "]";
}

Trajectory_Segment::~Trajectory_Segment()
{
    if (aggregate)
        {
            delete motprof;
            delete geom;
        }
}
Path* Trajectory_Segment::GetPath() {
	return geom;
}

VelocityProfile* Trajectory_Segment::GetProfile() {
	return motprof;
}


}
