#include "DrawView.h"
// inclusion of the generated files (generated out of DrawViewPy.xml)
#include <Mod/TechDraw/App/DrawViewPy.h>
#include <Mod/TechDraw/App/DrawViewPy.cpp>
#include <Mod/TechDraw/App/DrawPagePy.h>

using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawViewPy::representation(void) const
{
    return std::string("<DrawView object>");
}

//! replace the current Label with a translated version
PyObject* DrawViewPy::translateLabel(PyObject *args)
{
    PyObject* pyContext;
    PyObject* pyBaseName;
    PyObject* pyUniqueName;
    std::string context;
    std::string baseName;
    std::string uniqueName;

    if (!PyArg_ParseTuple(args, "OOO", &pyContext, &pyBaseName, &pyUniqueName)) {
            throw Py::TypeError("Could not translate label - bad parameters.");
    }

    Py_ssize_t size = 0;
    const char* cContext = PyUnicode_AsUTF8AndSize(pyContext, &size);
    if (cContext) {
        context = std::string(cContext, size);
    } else {
        throw Py::TypeError("Could not translate label - context not available.");
    }

    const char* cBaseName = PyUnicode_AsUTF8AndSize(pyBaseName, &size);
    if (cBaseName) {
        baseName = std::string(cBaseName, size);
    } else {
        throw Py::TypeError("Could not translate label - base name not available.");
    }

    const char* cUniqueName = PyUnicode_AsUTF8AndSize(pyUniqueName, &size);
    if (cUniqueName) {
        uniqueName = std::string(cUniqueName, size);
    } else {
        throw Py::TypeError("Could not translate label - unique name not available.");
    }

    // we have the 3 parameters we need for DrawView::translateLabel
    DrawView* dv = getDrawViewPtr();
    dv->translateLabel(context, baseName, uniqueName);

    Py_Return;
}

//! return the correct scale for this view
PyObject* DrawViewPy::getScale(PyObject *args) const
{
    if (!PyArg_ParseTuple(args, "")) {
            throw Py::TypeError("Do not understand passed parameter.");
    }

    DrawView* dv = getDrawViewPtr();

    return PyFloat_FromDouble(dv->getScale());
}

//! return the correct scale for this view
PyObject* DrawViewPy::findParentPage(PyObject *args) const
{
    if (!PyArg_ParseTuple(args, "")) {
            throw Py::TypeError("Do not understand passed parameter.");
    }

    DrawView* dv = getDrawViewPtr();
    DrawPage* parent = dv->findParentPage();

    return Py::new_reference_to(parent->getPyObject());
}


PyObject *DrawViewPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawViewPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
