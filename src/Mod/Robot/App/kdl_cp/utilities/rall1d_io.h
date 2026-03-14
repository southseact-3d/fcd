#ifndef Rall_IO_H
#define Rall_IO_H

#include "utility_io.h"
#include "rall1d.h"

namespace KDL {

template <class T,class V,class S>
inline std::ostream& operator << (std::ostream& os,const Rall1d<T,V,S>& r)
            {
            os << "Rall1d(" << r.t <<"," << r.grad <<")";
            return os;
            }


}


#endif
