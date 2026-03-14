// inclusion of the generated files (generated out of JointGroup.xml)
#include "JointGroupPy.h"
#include "JointGroupPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string JointGroupPy::representation() const
{
    return {"<Joint Group>"};
}

PyObject* JointGroupPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int JointGroupPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
