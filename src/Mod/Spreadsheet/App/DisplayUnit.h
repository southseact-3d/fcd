#ifndef DISPLAYUNIT_H
#define DISPLAYUNIT_H

#include <Base/Unit.h>
#include <string>

namespace Spreadsheet
{

class DisplayUnit
{
public:
    std::string stringRep;
    Base::Unit unit;
    double scaler;

    explicit DisplayUnit(
        const std::string _stringRep = "",
        const Base::Unit _unit = Base::Unit(),
        double _scaler = 0.0
    )
        : stringRep(std::move(_stringRep))
        , unit(_unit)
        , scaler(_scaler)
    {}

    bool operator==(const DisplayUnit& c) const
    {
        return c.stringRep == stringRep && c.unit == unit && c.scaler == scaler;
    }

    bool operator!=(const DisplayUnit& c) const
    {
        return !operator==(c);
    }

    bool isEmpty() const
    {
        return stringRep.empty();
    }
};

}  // namespace Spreadsheet

#endif  // DISPLAYUNIT_H
