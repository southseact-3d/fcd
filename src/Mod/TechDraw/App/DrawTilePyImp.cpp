#include "DrawTile.h"
// inclusion of the generated files (generated out of DrawTilePy.xml)
#include <Mod/TechDraw/App/DrawTilePy.h>
#include <Mod/TechDraw/App/DrawTilePy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawTilePy::representation() const
{
    return std::string("<DrawTile object>");
}

PyObject *DrawTilePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawTilePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
