// inclusion of the generated files (generated out of PropertyContainerPy.xml)
#include "DocumentObjectExtensionPy.h"
#include "DocumentObjectExtensionPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string DocumentObjectExtensionPy::representation() const
{
    return {"<document object extension>"};
}

PyObject* DocumentObjectExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DocumentObjectExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
