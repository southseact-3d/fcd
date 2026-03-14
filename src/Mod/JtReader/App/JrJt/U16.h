#ifndef U16_HEADER
#define U16_HEADER

#include "Context.h"
#include <istream>
#include <stdint.h>

using namespace std;

struct U16
{
    U16() {};

    U16(uint16_t ui)
        : _U16(ui)
    {}

    U16(Context& cont)
    {
        read(cont);
    }

    inline void read(Context& cont)
    {
        cont.Strm.read((char*)&_U16, 2);
    }

    inline operator uint16_t() const
    {
        return _U16;
    }

    uint16_t _U16;
};


#endif
