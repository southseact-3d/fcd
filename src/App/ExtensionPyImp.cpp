#include "ExtensionContainer.h"

// inclusion of the generated files (generated out of ExtensionPy.pyi)
#include <App/ExtensionPy.h>
#include <App/ExtensionPy.cpp>

using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string ExtensionPy::representation() const
{
    return {"<extension>"};
}

Py::Object ExtensionPy::getExtendedObject() const
{
    return Py::Object(getExtensionPtr()->getExtendedContainer()->getPyObject(), true);
}

PyObject* ExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
