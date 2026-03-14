#ifndef U8_HEADER
#define U8_HEADER

#include "Context.h"
#include <istream>
#include <stdint.h>

using namespace std;

struct U8
{
    U8() {};

    U8(uint8_t ui)
        : _U8(ui)
    {}

    U8(Context& cont)
    {
        read(cont);
    }

    inline void read(Context& cont)
    {
        cont.Strm.read((char*)&_U8, 1);
    }

    inline operator uint8_t() const
    {
        return _U8;
    }


    uint8_t _U8;
};


#endif
