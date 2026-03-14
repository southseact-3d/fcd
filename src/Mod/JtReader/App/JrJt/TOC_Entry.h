#ifndef TOC_Entry_HEADER
#define TOC_Entry_HEADER

#include <istream>
#include <stdint.h>

#include "Context.h"
#include "GUID.h"
#include "I32.h"
#include "U16.h"
#include "U32.h"


using namespace std;


struct TOC_Entry
{
    TOC_Entry() {};

    TOC_Entry(Context& cont)
    {
        read(cont);
    };

    inline void read(Context& cont)
    {
        Segment_ID.read(cont);
        Segment_Offset.read(cont);
        Segment_Length.read(cont);
        Segment_Attributes.read(cont);
    };

    uint8_t getSegmentType() const
    {
        return Segment_Attributes >> 24;
    }

    std::string toString() const
    {
        stringstream strm;
        strm << getSegmentType() << ":" << Segment_Offset << ":" << Segment_Length << ":"
             << Segment_ID.toString();

        return strm.str();
    }

    GUID Segment_ID;
    I32 Segment_Offset;
    I32 Segment_Length;
    U32 Segment_Attributes;
};


#endif
