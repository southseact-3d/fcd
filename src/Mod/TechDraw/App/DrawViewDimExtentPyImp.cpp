#include "DrawViewDimExtent.h"
// inclusion of the generated files (generated out of DrawViewDimExtentPy.xml)
#include <Mod/TechDraw/App/DrawViewDimExtentPy.h>
#include <Mod/TechDraw/App/DrawViewDimExtentPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawViewDimExtentPy::representation() const
{
    return std::string("<DrawViewDimExtent object>");
}
PyObject* DrawViewDimExtentPy::tbd(PyObject* args)
{
    (void) args;
    return nullptr;
}



PyObject *DrawViewDimExtentPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawViewDimExtentPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
