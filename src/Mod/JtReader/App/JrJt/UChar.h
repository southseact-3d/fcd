#ifndef UChar_HEADER
#define UChar_HEADER

#include "Context.h"
#include <istream>
#include <stdint.h>

using namespace std;

struct UChar
{
    UChar() {};

    UChar(Context& cont)
    {
        read(cont);
    }

    inline operator uint8_t() const
    {
        return _UChar;
    }

    inline void read(Context& cont)
    {
        cont.Strm.read((char*)&_UChar, 1);
    }

    uint8_t _UChar;
};


#endif
