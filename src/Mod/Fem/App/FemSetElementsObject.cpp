#include <App/DocumentObjectPy.h>

#include "FemSetElementsObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetElementsObject, Fem::FemSetObject)


FemSetElementsObject::FemSetElementsObject() = default;

FemSetElementsObject::~FemSetElementsObject() = default;

short FemSetElementsObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetElementsObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
