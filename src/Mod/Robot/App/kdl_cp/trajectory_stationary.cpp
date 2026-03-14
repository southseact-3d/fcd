#include "trajectory_stationary.hpp"

namespace KDL {

    using namespace std;


void Trajectory_Stationary::Write(ostream& os) const {
	os << "STATIONARY[ " << duration << endl;
	os << pos << endl;
	os << "]";
}


}

