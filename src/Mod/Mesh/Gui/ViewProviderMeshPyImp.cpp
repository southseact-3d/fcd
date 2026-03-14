#include <sstream>


#include "ViewProvider.h"
// inclusion of the generated files (generated out of ViewProviderMeshPy.xml)
#include "ViewProviderMeshPy.h"
#include "ViewProviderMeshPy.cpp"


using namespace MeshGui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderMeshPy::representation() const
{
    std::stringstream str;
    str << "<View provider object at " << getViewProviderDocumentObjectPtr() << ">";

    return str.str();
}

PyObject* ViewProviderMeshPy::setSelection(PyObject* args)
{
    PyObject* obj {};
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return nullptr;
    }

    Py::Sequence list(obj);
    std::vector<Mesh::FacetIndex> selection;
    selection.reserve(list.size());
    for (Py::Sequence::iterator it = list.begin(); it != list.end(); ++it) {
        Py::Long index(*it);
        auto value = static_cast<Mesh::FacetIndex>(index);
        selection.push_back(value);
    }

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->setSelection(selection);
    Py_Return;
}

PyObject* ViewProviderMeshPy::addSelection(PyObject* args)
{
    PyObject* obj {};
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return nullptr;
    }

    Py::Sequence list(obj);
    std::vector<Mesh::FacetIndex> selection;
    selection.reserve(list.size());
    for (Py::Sequence::iterator it = list.begin(); it != list.end(); ++it) {
        Py::Long index(*it);
        auto value = static_cast<Mesh::FacetIndex>(index);
        selection.push_back(value);
    }

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->addSelection(selection);
    Py_Return;
}

PyObject* ViewProviderMeshPy::removeSelection(PyObject* args)
{
    PyObject* obj {};
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return nullptr;
    }

    Py::Sequence list(obj);
    std::vector<Mesh::FacetIndex> selection;
    selection.reserve(list.size());
    for (Py::Sequence::iterator it = list.begin(); it != list.end(); ++it) {
        Py::Long index(*it);
        auto value = static_cast<Mesh::FacetIndex>(index);
        selection.push_back(value);
    }

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->removeSelection(selection);
    Py_Return;
}

PyObject* ViewProviderMeshPy::invertSelection(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->invertSelection();
    Py_Return;
}

PyObject* ViewProviderMeshPy::clearSelection(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->clearSelection();
    Py_Return;
}

PyObject* ViewProviderMeshPy::highlightSegments(PyObject* args)
{
    PyObject* list {};
    if (!PyArg_ParseTuple(args, "O", &list)) {
        return nullptr;
    }

    App::PropertyColorList colors;
    colors.setPyObject(list);

    ViewProviderMesh* vp = getViewProviderMeshPtr();
    vp->highlightSegments(colors.getValues());
    Py_Return;
}

PyObject* ViewProviderMeshPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderMeshPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
