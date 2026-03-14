// inclusion of the generated files (generated out of OriginGroupPy.xml)
#include "OriginGroupExtensionPy.h"
#include "OriginGroupExtensionPy.cpp"

using namespace App;

// returns a string which represents the object e.g. when printed in python
std::string OriginGroupExtensionPy::representation() const
{
    return {"<OriginGroup object>"};
}

PyObject* OriginGroupExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int OriginGroupExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
