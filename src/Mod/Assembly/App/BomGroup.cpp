#include <App/Application.h>
#include <App/Document.h>
#include <App/FeaturePythonPyImp.h>
#include <App/PropertyPythonObject.h>
#include <Base/Console.h>
#include <Base/Tools.h>

#include "BomGroup.h"
#include "BomGroupPy.h"

using namespace Assembly;


PROPERTY_SOURCE(Assembly::BomGroup, App::DocumentObjectGroup)

BomGroup::BomGroup()
{}

BomGroup::~BomGroup() = default;

PyObject* BomGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new BomGroupPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
