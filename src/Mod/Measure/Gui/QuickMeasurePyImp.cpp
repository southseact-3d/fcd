#include <App/Application.h>
#include <App/Document.h>
#include <Base/GeometryPyCXX.h>

// inclusion of the generated files (generated out of QuickMeasurePy.xml)
#include "QuickMeasurePy.h"
#include "QuickMeasurePy.cpp"


using namespace MeasureGui;

// returns a string which represents the object e.g. when printed in python
std::string QuickMeasurePy::representation() const
{
    return "<MeasureGui::QuickMeasure>";
}

PyObject* QuickMeasurePy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of BoundBoxPy and the Twin object
    return new QuickMeasurePy(new QuickMeasure);
}

// constructor method
int QuickMeasurePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* QuickMeasurePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int QuickMeasurePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
