#ifndef Context_HEADER
#define Context_HEADER

#include <istream>
#include <stdint.h>

using namespace std;

class Context
{
public:
    Context(istream& s)
        : Strm(s) {};
    uint8_t VersionMinor;
    uint8_t VersionMajor;

    istream& Strm;
};


#endif
