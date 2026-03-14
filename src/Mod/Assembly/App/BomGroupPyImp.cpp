// inclusion of the generated files (generated out of BomGroup.xml)
#include "BomGroupPy.h"
#include "BomGroupPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string BomGroupPy::representation() const
{
    return {"<Bom Group>"};
}

PyObject* BomGroupPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int BomGroupPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
