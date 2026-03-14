#ifndef Segment_Header_HEADER
#define Segment_Header_HEADER

#include <istream>
#include <stdint.h>

#include "Context.h"
#include "GUID.h"
#include "I32.h"
#include "U16.h"
#include "U32.h"


using namespace std;


struct Segment_Header
{
    Segment_Header() {};

    Segment_Header(Context& cont)
    {
        read(cont);
    };

    inline void read(Context& cont)
    {
        Segment_ID.read(cont);
        Segment_Type.read(cont);
        Segment_Length.read(cont);
    };

    GUID Segment_ID;
    I32 Segment_Type;
    I32 Segment_Length;
};


#endif
