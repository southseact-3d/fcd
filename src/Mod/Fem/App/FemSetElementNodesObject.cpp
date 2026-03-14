#include <App/DocumentObjectPy.h>

#include "FemSetElementNodesObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetElementNodesObject, Fem::FemSetObject)

FemSetElementNodesObject::FemSetElementNodesObject()
{
    ADD_PROPERTY_TYPE(Elements, (), "Element indexes", Prop_None, "Elements belonging to the ElementSet");
}

FemSetElementNodesObject::~FemSetElementNodesObject() = default;

short FemSetElementNodesObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetElementNodesObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
