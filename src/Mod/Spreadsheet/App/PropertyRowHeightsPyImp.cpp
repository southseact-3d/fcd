#include "PropertyRowHeights.h"
// inclusion of the generated files (generated out of PropertyRowHeightsPy.xml)
// clang-format off
#include "PropertyRowHeightsPy.h"
#include "PropertyRowHeightsPy.cpp"
// clang-format on


using namespace Spreadsheet;

// returns a string which represents the object e.g. when printed in python
std::string PropertyRowHeightsPy::representation() const
{
    return {"<PropertyRowHeights object>"};
}

PyObject* PropertyRowHeightsPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of PropertyRowHeightsPy and the Twin object
    return new PropertyRowHeightsPy(new PropertyRowHeights);
}

// constructor method
int PropertyRowHeightsPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* PropertyRowHeightsPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int PropertyRowHeightsPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
