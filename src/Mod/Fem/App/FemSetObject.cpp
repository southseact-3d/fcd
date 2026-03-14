#include "FemSetObject.h"
#include <App/DocumentObjectPy.h>


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSetObject, App::DocumentObject)


FemSetObject::FemSetObject()
{
    ADD_PROPERTY_TYPE(FemMesh, (nullptr), "MeshSet link", Prop_None, "MeshSet the set belongs to");
}

FemSetObject::~FemSetObject() = default;

short FemSetObject::mustExecute() const
{
    return 0;
}

PyObject* FemSetObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
