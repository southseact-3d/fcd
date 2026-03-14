#include <App/Application.h>
#include <App/Document.h>
#include <App/FeaturePythonPyImp.h>
#include <App/PropertyPythonObject.h>
#include <Base/Console.h>
#include <Base/Tools.h>

#include "SimulationGroup.h"
#include "SimulationGroupPy.h"

using namespace Assembly;


PROPERTY_SOURCE(Assembly::SimulationGroup, App::DocumentObjectGroup)

SimulationGroup::SimulationGroup()
{}

SimulationGroup::~SimulationGroup() = default;

PyObject* SimulationGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new SimulationGroupPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
