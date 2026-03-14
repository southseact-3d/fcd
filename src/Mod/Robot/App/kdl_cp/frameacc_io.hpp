#ifndef RRFRAMES_IO
#define RRFRAMES_IO

#include "utilities/utility_io.h"
#include "utilities/rall2d_io.h"

#include "frames_io.hpp"
#include "frameacc.hpp"

namespace KDL {


// Output...
inline std::ostream& operator << (std::ostream& os,const VectorAcc& r) {
    os << "{" << r.p << "," << r.v << "," << r.dv << "}" << std::endl;
    return os;
}

inline std::ostream& operator << (std::ostream& os,const RotationAcc& r) {
    os << "{" << std::endl << r.R << "," << std::endl << r.w <<
          "," << std::endl << r.dw << std::endl << "}" << std::endl;
    return os;
}


inline std::ostream& operator << (std::ostream& os,const FrameAcc& r) {
    os << "{" << std::endl << r.M << "," << std::endl << r.p << "}" << std::endl;
    return os;
}
inline std::ostream& operator << (std::ostream& os,const TwistAcc& r) {
    os << "{" << std::endl << r.vel << "," << std::endl << r.rot << std::endl << "}" << std::endl;
    return os;
}


} // namespace Frame



#endif
