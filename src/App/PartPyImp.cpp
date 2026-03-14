// inclusion of the generated files (generated out of PartPy.xml)
#include "PartPy.h"
#include "PartPy.cpp"

using namespace App;

// returns a string which represents the object e.g. when printed in python
std::string PartPy::representation() const
{
    return {"<Part object>"};
}

PyObject* PartPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int PartPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
