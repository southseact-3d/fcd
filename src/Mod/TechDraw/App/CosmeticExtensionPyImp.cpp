#include "Mod/TechDraw/App/CosmeticExtension.h"
// inclusion of the generated files (generated out of CosmeticExtensionPy.xml)
#include "CosmeticExtensionPy.h"
#include "CosmeticExtensionPy.cpp"


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string CosmeticExtensionPy::representation() const
{
    return std::string("<TechDraw::CosmeticObject>");
}

PyObject *CosmeticExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int CosmeticExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}


