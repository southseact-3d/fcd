// inclusion of the generated files (generated out of BomObject.xml)
#include "BomObjectPy.h"
#include "BomObjectPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string BomObjectPy::representation() const
{
    return {"<Bill of Materials object>"};
}

PyObject* BomObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int BomObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
