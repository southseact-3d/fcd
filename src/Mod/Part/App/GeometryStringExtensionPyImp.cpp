#include <sstream>


#include "GeometryStringExtensionPy.h"
#include "GeometryStringExtensionPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryStringExtensionPy::representation() const
{
    std::stringstream str;
    str << "<GeometryStringExtension (";

    if (!getGeometryStringExtensionPtr()->getName().empty()) {
        str << "\'" << getGeometryStringExtensionPtr()->getName() << "\', ";
    }

    str << getGeometryStringExtensionPtr()->getValue() << ") >";

    return str.str();
}

PyObject* GeometryStringExtensionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of the python object and the Twin object
    return new GeometryStringExtensionPy(new GeometryStringExtension);
}

// constructor method
int GeometryStringExtensionPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{

    if (PyArg_ParseTuple(args, "")) {
        // default extension
        return 0;
    }

    PyErr_Clear();
    char* pstr;
    if (PyArg_ParseTuple(args, "s", &pstr)) {
        this->getGeometryStringExtensionPtr()->setValue(pstr);
        return 0;
    }

    PyErr_Clear();
    char* pystr;
    if (PyArg_ParseTuple(args, "ss", &pstr, &pystr)) {
        this->getGeometryStringExtensionPtr()->setValue(pstr);
        this->getGeometryStringExtensionPtr()->setName(pystr);
        return 0;
    }


    PyErr_SetString(
        PyExc_TypeError,
        "GeometryStringExtension constructor accepts:\n"
        "-- empty parameter list\n"
        "-- string\n"
        "-- string, string\n"
    );
    return -1;
}

Py::String GeometryStringExtensionPy::getValue() const
{
    return {this->getGeometryStringExtensionPtr()->getValue()};
}

void GeometryStringExtensionPy::setValue(Py::String value)
{
    this->getGeometryStringExtensionPtr()->setValue(value.as_std_string());
}

PyObject* GeometryStringExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeometryStringExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
