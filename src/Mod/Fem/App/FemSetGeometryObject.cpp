#include "FemSetGeometryObject.h"
#include <App/DocumentObjectPy.h>


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetGeometryObject, Fem::FemSetObject)


FemSetGeometryObject::FemSetGeometryObject() = default;

FemSetGeometryObject::~FemSetGeometryObject() = default;

short FemSetGeometryObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetGeometryObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
