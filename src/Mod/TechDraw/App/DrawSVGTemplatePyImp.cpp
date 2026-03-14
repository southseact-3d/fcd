#include "DrawSVGTemplate.h"
// inclusion of the generated files (generated out of DrawSVGTemplatePy.xml)
#include <Mod/TechDraw/App/DrawSVGTemplatePy.h>
#include <Mod/TechDraw/App/DrawSVGTemplatePy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawSVGTemplatePy::representation() const
{
    return std::string("<DrawSVGTemplate object>");
}

//! replace the current Label with a translated version
PyObject* DrawSVGTemplatePy::translateLabel(PyObject *args)
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

    // we have the 3 parameters we need for DrawSVGTemplate::translateLabel
    auto svgTemplate = getDrawSVGTemplatePtr();
    svgTemplate->translateLabel(context, baseName, uniqueName);

    Py_Return;
}

PyObject *DrawSVGTemplatePy::getCustomAttributes(const char* ) const
{
    return nullptr;
}

int DrawSVGTemplatePy::setCustomAttributes(const char* attr, PyObject* obj)
{
    // search in PropertyList
    App::Property *prop = getDrawSVGTemplatePtr()->getPropertyByName(attr);
    if (prop) {
        // Read-only attributes must not be set over its Python interface
        short Type =  getDrawSVGTemplatePtr()->getPropertyType(prop);
        if (Type & App::Prop_ReadOnly) {
            std::stringstream s;
            s << "Object attribute '" << attr << "' is read-only";
            throw Py::AttributeError(s.str());
        }

        prop->setPyObject(obj);
        return 1;
    }

    return 0;
}

PyObject* DrawSVGTemplatePy::getEditFieldContent(PyObject* args)
{
    char* fieldName;
    if (!PyArg_ParseTuple(args, "s", &fieldName)) {
        return nullptr;
    }
    std::string content = getDrawSVGTemplatePtr()->EditableTexts[fieldName];
    if (!content.empty()) {
        return PyUnicode_FromString(content.c_str());
    }
    Py_Return;
}

PyObject* DrawSVGTemplatePy::setEditFieldContent(PyObject* args)
{
    char* fieldName;
    char* newContent;
    if (!PyArg_ParseTuple(args, "ss", &fieldName, &newContent)) {
        return nullptr;
    }

    getDrawSVGTemplatePtr()->EditableTexts.setValue(fieldName, newContent);

    Py_Return;
}
