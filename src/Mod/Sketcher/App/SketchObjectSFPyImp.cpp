// inclusion of the generated files (generated out of SketchObjectSFPy.xml)
#include "SketchObjectSFPy.h"

#include "SketchObjectSFPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchObjectSFPy::representation() const
{
    return "<SketchObjectSF object>";
}

PyObject* SketchObjectSFPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int SketchObjectSFPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
