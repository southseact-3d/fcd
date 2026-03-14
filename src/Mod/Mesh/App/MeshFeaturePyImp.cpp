#include "MeshFeature.h"
// inclusion of the generated files (generated out of MeshFeaturePy.xml)
// clang-format off
#include <Mod/Mesh/App/MeshPy.h>
#include <Mod/Mesh/App/MeshFeaturePy.h>
#include <Mod/Mesh/App/MeshFeaturePy.cpp>
// clang-format on


using namespace Mesh;


// returns a string which represent the object e.g. when printed in python
std::string MeshFeaturePy::representation() const
{
    std::stringstream str;
    str << getFeaturePtr()->getTypeId().getName() << " object at " << getFeaturePtr();

    return str.str();
}

PyObject* MeshFeaturePy::countPoints(PyObject* /*args*/)
{
    return Py_BuildValue("i", getFeaturePtr()->Mesh.getValue().countPoints());
}

PyObject* MeshFeaturePy::countFacets(PyObject* /*args*/)
{
    return Py_BuildValue("i", getFeaturePtr()->Mesh.getValue().countFacets());
}

PyObject* MeshFeaturePy::harmonizeNormals(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::MeshObject* mesh = getFeaturePtr()->Mesh.startEditing();
        mesh->harmonizeNormals();
        getFeaturePtr()->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::smooth(PyObject* args)
{
    int iter = 1;
    float d_max = std::numeric_limits<float>::max();
    if (!PyArg_ParseTuple(args, "|if", &iter, &d_max)) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->smooth(iter, d_max);
        obj->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::decimate(PyObject* args)
{
    float fTol {};
    float fRed {};
    if (PyArg_ParseTuple(args, "ff", &fTol, &fRed)) {
        PY_TRY
        {
            Mesh::Feature* obj = getFeaturePtr();
            MeshObject* kernel = obj->Mesh.startEditing();
            kernel->decimate(fTol, fRed);
            obj->Mesh.finishEditing();
        }
        PY_CATCH;

        Py_Return;
    }

    PyErr_Clear();
    int targetSize {};
    if (PyArg_ParseTuple(args, "i", &targetSize)) {
        PY_TRY
        {
            Mesh::Feature* obj = getFeaturePtr();
            MeshObject* kernel = obj->Mesh.startEditing();
            kernel->decimate(targetSize);
            obj->Mesh.finishEditing();
        }
        PY_CATCH;

        Py_Return;
    }

    PyErr_SetString(
        PyExc_ValueError,
        "decimate(tolerance=float, reduction=float) or decimate(targetSize=int)"
    );
    return nullptr;
}

PyObject* MeshFeaturePy::removeNonManifolds(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    Mesh::Feature* obj = getFeaturePtr();
    MeshObject* kernel = obj->Mesh.startEditing();
    kernel->removeNonManifolds();
    obj->Mesh.finishEditing();
    Py_Return;
}

PyObject* MeshFeaturePy::removeNonManifoldPoints(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    Mesh::Feature* obj = getFeaturePtr();
    MeshObject* kernel = obj->Mesh.startEditing();
    kernel->removeNonManifoldPoints();
    obj->Mesh.finishEditing();
    Py_Return;
}

PyObject* MeshFeaturePy::fixIndices(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->validateIndices();
        obj->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::fixDegenerations(PyObject* args)
{
    float fEpsilon = MeshCore::MeshDefinitions::_fMinPointDistanceP2;
    if (!PyArg_ParseTuple(args, "|f", &fEpsilon)) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->validateDegenerations(fEpsilon);
        obj->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::removeDuplicatedFacets(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->removeDuplicatedFacets();
        obj->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::removeDuplicatedPoints(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    PY_TRY
    {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->removeDuplicatedPoints();
        obj->Mesh.finishEditing();
    }
    PY_CATCH;

    Py_Return;
}

PyObject* MeshFeaturePy::fixSelfIntersections(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    try {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->removeSelfIntersections();
        obj->Mesh.finishEditing();
    }
    catch (const Base::Exception& e) {
        e.setPyException();
        return nullptr;
    }
    Py_Return;
}

PyObject* MeshFeaturePy::removeFoldsOnSurface(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    try {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->removeFoldsOnSurface();
        obj->Mesh.finishEditing();
    }
    catch (const Base::Exception& e) {
        e.setPyException();
        return nullptr;
    }
    Py_Return;
}

PyObject* MeshFeaturePy::removeInvalidPoints(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    try {
        Mesh::Feature* obj = getFeaturePtr();
        MeshObject* kernel = obj->Mesh.startEditing();
        kernel->removeInvalidPoints();
        obj->Mesh.finishEditing();
    }
    catch (const Base::Exception& e) {
        e.setPyException();
        return nullptr;
    }
    Py_Return;
}

PyObject* MeshFeaturePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int MeshFeaturePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
