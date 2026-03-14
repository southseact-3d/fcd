#include "utilities/error.h"
#include "utilities/error_stack.h"
#include "trajectory.hpp"
#include "path.hpp"
#include "trajectory_segment.hpp"

#include <memory>
#include <cstring>


namespace KDL {

using namespace std;

Trajectory* Trajectory::Read(std::istream& is) {
	// unique_ptr because exception can be thrown !
	IOTrace("Trajectory::Read");
	char storage[64];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"SEGMENT")==0) {
		IOTrace("SEGMENT");
		unique_ptr<Path>      geom(    Path::Read(is)       );
		unique_ptr<VelocityProfile> motprof( VelocityProfile::Read(is)  );
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new  Trajectory_Segment(geom.release(),motprof.release());
	} else {
		throw Error_MotionIO_Unexpected_Traj();
	}
	return nullptr; // just to avoid the warning;
}



}

