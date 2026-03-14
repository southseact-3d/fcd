#include "DrawGeomHatch.h"
// inclusion of the generated files (generated out of DrawGeomHatchPy.xml)
#include <Mod/TechDraw/App/DrawGeomHatchPy.h>
#include <Mod/TechDraw/App/DrawGeomHatchPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawGeomHatchPy::representation() const
{
    return std::string("<DrawGeomHatch object>");
}

//! replace the current Label with a translated version
PyObject* DrawGeomHatchPy::translateLabel(PyObject *args)
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
    DrawGeomHatch* dgh = getDrawGeomHatchPtr();
    dgh->translateLabel(context, baseName, uniqueName);

    Py_Return;
}


PyObject *DrawGeomHatchPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawGeomHatchPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
