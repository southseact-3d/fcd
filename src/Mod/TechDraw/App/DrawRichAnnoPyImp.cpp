#include "DrawRichAnno.h"
// inclusion of the generated files (generated out of DrawRichAnnoPy.xml)
#include <Mod/TechDraw/App/DrawRichAnnoPy.h>
#include <Mod/TechDraw/App/DrawRichAnnoPy.cpp>

using namespace TechDraw;


// returns a string which represents the object e.g. when printed in python
std::string DrawRichAnnoPy::representation() const
{
    return std::string("<DrawRichAnno object>");
}

PyObject *DrawRichAnnoPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawRichAnnoPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
