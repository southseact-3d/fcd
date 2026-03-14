#ifndef I32_HEADER
#define I32_HEADER

#include "Context.h"
#include <istream>
#include <stdint.h>

using namespace std;

struct I32
{
    I32() {};

    I32(Context& cont)
    {
        read(cont);
    }

    inline operator int32_t() const
    {
        return _I32;
    }

    inline void read(Context& cont)
    {
        cont.Strm.read((char*)&_I32, 4);
    }

    int32_t _I32;
};


#endif
