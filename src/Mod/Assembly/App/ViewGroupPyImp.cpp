// inclusion of the generated files (generated out of ViewGroup.xml)
#include "ViewGroupPy.h"
#include "ViewGroupPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string ViewGroupPy::representation() const
{
    return {"<Exploded View Group>"};
}

PyObject* ViewGroupPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewGroupPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
