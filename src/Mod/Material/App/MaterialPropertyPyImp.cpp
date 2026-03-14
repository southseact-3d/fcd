#include "Model.h"
#include "PyVariants.h"
#include "ModelPropertyPy.h"
#include "MaterialPropertyPy.h"

#include "MaterialPropertyPy.cpp"

using namespace Materials;

// returns a string which represents the object e.g. when printed in python
std::string MaterialPropertyPy::representation() const
{
    std::stringstream str;
    str << "<MaterialProperty object at " << getMaterialPropertyPtr() << ">";

    return str.str();
}

PyObject* MaterialPropertyPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    return new MaterialPropertyPy(new MaterialProperty());
}

// constructor method
int MaterialPropertyPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

Py::Object MaterialPropertyPy::getValue() const
{
    auto value = getMaterialPropertyPtr()->getValue();

    return Py::Object(_pyObjectFromVariant(value), true);
}

Py::Boolean MaterialPropertyPy::getEmpty() const
{
    return getMaterialPropertyPtr()->isEmpty();
}

PyObject* MaterialPropertyPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int MaterialPropertyPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}