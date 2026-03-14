#ifndef U32_HEADER
#define U32_HEADER

#include "Context.h"
#include <istream>
#include <stdint.h>

using namespace std;

struct U32
{
    U32() {};

    U32(uint32_t ui)
        : _U32(ui)
    {}

    U32(Context& cont)
    {
        read(cont);
    }

    inline operator uint32_t() const
    {
        return _U32;
    }

    inline void read(Context& cont)
    {
        cont.Strm.read((char*)&_U32, 4);
    }

    uint32_t _U32;
};


#endif
