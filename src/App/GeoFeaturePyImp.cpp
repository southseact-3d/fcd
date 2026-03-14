// inclusion of the generated files (generated out of GeoFeaturePy.xml)
#include "GeoFeaturePy.h"
#include "GeoFeaturePy.cpp"
#include <Base/PlacementPy.h>
#include <CXX/Objects.hxx>

using namespace App;

// returns a string which represents the object e.g. when printed in python
std::string GeoFeaturePy::representation() const
{
    return {"<GeoFeature object>"};
}

PyObject* GeoFeaturePy::getPaths(PyObject* /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return nullptr;
}

PyObject* GeoFeaturePy::getGlobalPlacement(PyObject* args)
{

    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    try {
        Base::Placement p = static_cast<GeoFeature*>(getDocumentObjectPtr())->globalPlacement();
        return new Base::PlacementPy(new Base::Placement(p));
    }
    catch (const Base::Exception& e) {
        throw Py::RuntimeError(e.what());
    }
}

PyObject* GeoFeaturePy::getGlobalPlacementOf(PyObject* args)
{

    PyObject* pyTargetObj {nullptr};
    PyObject* pyRootObj {nullptr};
    char* pname;

    if (!PyArg_ParseTuple(args, "OOs", &pyTargetObj, &pyRootObj, &pname)) {
        return nullptr;
    }
    auto* targetObj = static_cast<App::DocumentObjectPy*>(pyTargetObj)->getDocumentObjectPtr();
    auto* rootObj = static_cast<App::DocumentObjectPy*>(pyRootObj)->getDocumentObjectPtr();

    try {
        Base::Placement p = GeoFeature::getGlobalPlacement(targetObj, rootObj, pname);
        return new Base::PlacementPy(new Base::Placement(p));
    }
    catch (const Base::Exception& e) {
        throw Py::RuntimeError(e.what());
    }
}

PyObject* GeoFeaturePy::getPropertyNameOfGeometry(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    GeoFeature* object = this->getGeoFeaturePtr();
    const PropertyComplexGeoData* prop = object->getPropertyOfGeometry();
    const char* name = prop ? prop->getName() : nullptr;
    if (Property::isValidName(name)) {
        return Py::new_reference_to(Py::String(std::string(name)));
    }
    return Py::new_reference_to(Py::None());
}

PyObject* GeoFeaturePy::getPropertyOfGeometry(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    GeoFeature* object = this->getGeoFeaturePtr();
    const PropertyComplexGeoData* prop = object->getPropertyOfGeometry();
    if (prop) {
        return const_cast<PropertyComplexGeoData*>(prop)->getPyObject();
    }
    return Py::new_reference_to(Py::None());
}

PyObject* GeoFeaturePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeoFeaturePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}

Py::String GeoFeaturePy::getElementMapVersion() const
{
    return Py::String(
        getGeoFeaturePtr()->getElementMapVersion(getGeoFeaturePtr()->getPropertyOfGeometry()));
}
