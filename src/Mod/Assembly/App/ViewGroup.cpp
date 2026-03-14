#include <App/Application.h>
#include <App/Document.h>
#include <App/FeaturePythonPyImp.h>
#include <App/PropertyPythonObject.h>
#include <Base/Console.h>
#include <Base/Tools.h>

#include "ViewGroup.h"
#include "ViewGroupPy.h"

using namespace Assembly;


PROPERTY_SOURCE(Assembly::ViewGroup, App::DocumentObjectGroup)

ViewGroup::ViewGroup()
{}

ViewGroup::~ViewGroup() = default;

PyObject* ViewGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new ViewGroupPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
