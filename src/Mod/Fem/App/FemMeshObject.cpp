#include <App/FeaturePythonPyImp.h>
#include <App/GeoFeaturePy.h>
#include <Base/Placement.h>

#include "FemMesh.h"
#include "FemMeshObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemMeshObject, App::GeoFeature)


FemMeshObject::FemMeshObject()
{
    ADD_PROPERTY_TYPE(FemMesh, (), "FEM Mesh", Prop_NoRecompute, "FEM Mesh object");
    // in the regard of recomputes see:
    // https://forum.freecad.org/viewtopic.php?f=18&t=33329#p279203

    suppressibleExt.initExtension(this);
}

FemMeshObject::~FemMeshObject() = default;

short FemMeshObject::mustExecute() const
{
    return 0;
}

PyObject* FemMeshObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::asObject(new GeoFeaturePy(this));
    }
    return Py::new_reference_to(PythonObject);
}

void FemMeshObject::onChanged(const Property* prop)
{
    App::GeoFeature::onChanged(prop);

    // if the placement has changed apply the change to the mesh data as well
    if (prop == &this->Placement) {
        this->FemMesh.setTransform(this->Placement.getValue().toMatrix());
    }
}

// Python feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Fem::FemMeshObjectPython, Fem::FemMeshObject)
template<>
const char* Fem::FemMeshObjectPython::getViewProviderName() const
{
    return "FemGui::ViewProviderFemMeshPython";
}

template<>
PyObject* Fem::FemMeshObjectPython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::asObject(new App::FeaturePythonPyT<App::GeoFeaturePy>(this));
    }
    return Py::new_reference_to(PythonObject);
}

// explicit template instantiation
template class FemExport FeaturePythonT<Fem::FemMeshObject>;

/// @endcond

}  // namespace App
