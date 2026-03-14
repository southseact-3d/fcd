#include "DocumentObjectGroup.h"

// inclusion of the generated files (generated out of DocumentObjectGroupPy.xml)
#include "DocumentObjectGroupPy.h"
#include "DocumentObjectGroupPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string DocumentObjectGroupPy::representation() const
{
    return {"<group object>"};
}

PyObject* DocumentObjectGroupPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DocumentObjectGroupPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
