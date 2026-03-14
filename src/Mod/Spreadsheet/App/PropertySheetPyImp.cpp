#include "PropertySheet.h"
// inclusion of the generated files (generated out of PropertySheetPy.xml)
// clang-format off
#include "PropertySheetPy.h"
#include "PropertySheetPy.cpp"
// clang-format on


using namespace Spreadsheet;

// returns a string which represents the object e.g. when printed in python
std::string PropertySheetPy::representation() const
{
    return {"<PropertySheet object>"};
}

PyObject* PropertySheetPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of PropertySheetPy and the Twin object
    return new PropertySheetPy(new PropertySheet);
}

// constructor method
int PropertySheetPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* PropertySheetPy::mapping_subscript(PyObject* o, PyObject* key)
{
    return static_cast<PropertySheetPy*>(o)->getPropertySheetPtr()->getPyValue(key);
}

PyObject* PropertySheetPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int PropertySheetPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
