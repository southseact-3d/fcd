#ifndef Rall2d_IO_H
#define Rall2d_IO_H



#include "utility_io.h"
#include "rall2d.h"

namespace KDL {

template <class T,class V,class S>
std::ostream& operator << (std::ostream& os,const Rall2d<T,V,S>& r)
            {
            os << "Rall2d(" << r.t <<"," << r.d <<","<<r.dd<<")";
            return os;
            }


}

#endif
