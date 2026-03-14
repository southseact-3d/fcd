#include "DrawTileWeld.h"
// inclusion of the generated files (generated out of DrawTileWeldPy.xml)
#include <Mod/TechDraw/App/DrawTileWeldPy.h>
#include <Mod/TechDraw/App/DrawTileWeldPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawTileWeldPy::representation(void) const
{
    return std::string("<DrawTileWeld object>");
}

PyObject *DrawTileWeldPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawTileWeldPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
