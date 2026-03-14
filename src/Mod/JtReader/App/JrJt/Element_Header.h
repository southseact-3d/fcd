#ifndef Element_Header_HEADER
#define Element_Header_HEADER

#include <assert.h>
#include <istream>
#include <stdint.h>

#include "Context.h"
#include "GUID.h"
#include "I32.h"
#include "UChar.h"


using namespace std;


struct Element_Header
{
    Element_Header() {};

    Element_Header(Context& cont, bool zLib = false)
    {
        read(cont, zLib);
    };

    inline void read(Context& cont, bool zLib = false)
    {
        // only zip less implemented so far...
        assert(zLib == false);

        Element_Length.read(cont);
        Object_Type_ID.read(cont);
        Object_Base_Type.read(cont);
    };

    I32 Element_Length;
    GUID Object_Type_ID;
    UChar Object_Base_Type;
};


#endif
