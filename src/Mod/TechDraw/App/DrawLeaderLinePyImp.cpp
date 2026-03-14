#include "DrawLeaderLine.h"
// inclusion of the generated files (generated out of DrawLeaderLinePy.xml)
#include <Mod/TechDraw/App/DrawLeaderLinePy.h>
#include <Mod/TechDraw/App/DrawLeaderLinePy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawLeaderLinePy::representation() const
{
    return std::string("<DrawLeaderLine object>");
}

PyObject *DrawLeaderLinePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawLeaderLinePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
