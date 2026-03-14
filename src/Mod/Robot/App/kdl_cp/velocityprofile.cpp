#include "utilities/error.h"
#include "utilities/error_stack.h"
#include "velocityprofile_rect.hpp"
#include "velocityprofile_dirac.hpp"
#include "velocityprofile_trap.hpp"
#include "velocityprofile_traphalf.hpp"
#include <string.h>

namespace KDL {

using namespace std;

VelocityProfile* VelocityProfile::Read(istream& is) {
	IOTrace("VelocityProfile::Read");
	char storage[25];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"DIRACVEL")==0) {
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_Dirac();
	} else if (strcmp(storage,"CONSTVEL")==0) {
		double vel;
		is >> vel;
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_Rectangular(vel);
	} else if (strcmp(storage,"TRAPEZOIDAL")==0) {
		double maxvel;
		double maxacc;
 		is >> maxvel;
		Eat(is,',');
		is >> maxacc;
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_Trap(maxvel,maxacc);
	} else if (strcmp(storage,"TRAPEZOIDALHALF")==0) {
		double maxvel;
		double maxacc;
 		is >> maxvel;
		Eat(is,',');
		is >> maxacc;
		Eat(is,',');
		bool starting;
		is >> starting;
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_TrapHalf(maxvel,maxacc,starting);
	}
	else {
		throw Error_MotionIO_Unexpected_MotProf();
	}
    return nullptr;
}



}
