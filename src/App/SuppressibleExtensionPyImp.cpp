#include "DocumentObject.h"

// inclusion of the generated files (generated out of SuppressibleExtensionPy.xml)
#include "SuppressibleExtensionPy.h"
#include "SuppressibleExtensionPy.cpp"
#include "DocumentObjectPy.h"


using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string SuppressibleExtensionPy::representation() const
{
    return {"<suppressible extension object>"};
}

PyObject* SuppressibleExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int SuppressibleExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
