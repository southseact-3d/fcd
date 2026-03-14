#include "PropertyColumnWidths.h"
// inclusion of the generated files (generated out of PropertyColumnWidthsPy.xml)
// clang-format off
#include "PropertyColumnWidthsPy.h"
#include "PropertyColumnWidthsPy.cpp"
// clang-format on


using namespace Spreadsheet;

// returns a string which represents the object e.g. when printed in python
std::string PropertyColumnWidthsPy::representation() const
{
    return {"<PropertyColumnWidths object>"};
}

PyObject* PropertyColumnWidthsPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of PropertyColumnWidthsPy and the Twin object
    return new PropertyColumnWidthsPy(new PropertyColumnWidths);
}

// constructor method
int PropertyColumnWidthsPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* PropertyColumnWidthsPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int PropertyColumnWidthsPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
