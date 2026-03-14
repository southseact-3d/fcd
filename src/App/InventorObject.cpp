#include "InventorObject.h"
#include "DocumentObjectPy.h"


using namespace App;

PROPERTY_SOURCE(App::InventorObject, App::GeoFeature)


InventorObject::InventorObject()
{
    ADD_PROPERTY_TYPE(Buffer, (""), "", Prop_None, "String buffer with a scene graph");
    ADD_PROPERTY_TYPE(FileName, (""), "", Prop_None, "Path to an Inventor file");
}

InventorObject::~InventorObject() = default;

short InventorObject::mustExecute() const
{
    return 0;
}

PyObject* InventorObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
