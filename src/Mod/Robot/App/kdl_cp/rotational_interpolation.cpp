#include "utilities/error.h"
#include "utilities/error_stack.h"
#include "rotational_interpolation.hpp"
#include "rotational_interpolation_sa.hpp"
#include <memory>
#include <cstring>

namespace KDL {

using namespace std;

RotationalInterpolation* RotationalInterpolation::Read(istream& is) {
	// unique_ptr because exception can be thrown !
	IOTrace("RotationalInterpolation::Read");
	char storage[64];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"SINGLEAXIS")==0) {
		IOTrace("SINGLEAXIS");
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new RotationalInterpolation_SingleAxis();
	} else if (strcmp(storage,"THREEAXIS")==0) {
		IOTrace("THREEAXIS");
		throw Error_Not_Implemented();
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return nullptr;
	} else if (strcmp(storage,"TWOAXIS")==0) {
		IOTrace("TWOAXIS");
		throw Error_Not_Implemented();
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return nullptr;
	} else {
		throw Error_MotionIO_Unexpected_Traj();
	}
	return nullptr; // just to avoid the warning;
}

}

