#include <App/DocumentObjectPy.h>

#include "FemSetFacesObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetFacesObject, Fem::FemSetObject)


FemSetFacesObject::FemSetFacesObject() = default;

FemSetFacesObject::~FemSetFacesObject() = default;

short FemSetFacesObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetFacesObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
