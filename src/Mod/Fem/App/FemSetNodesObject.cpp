#include <App/DocumentObjectPy.h>

#include "FemSetNodesObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetNodesObject, Fem::FemSetObject)


FemSetNodesObject::FemSetNodesObject()
{
    ADD_PROPERTY_TYPE(Nodes, (), "Node indexes", Prop_None, "Nodes belonging to the NodesSet");
}

FemSetNodesObject::~FemSetNodesObject() = default;

short FemSetNodesObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetNodesObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
