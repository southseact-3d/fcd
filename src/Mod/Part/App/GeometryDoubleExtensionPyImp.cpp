#include "GeometryDefaultExtension.h"

#include "GeometryDoubleExtensionPy.h"
#include "GeometryDoubleExtensionPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryDoubleExtensionPy::representation() const
{
    std::stringstream str;
    double val = getGeometryDoubleExtensionPtr()->getValue();
    str << "<GeometryDoubleExtension (";

    if (!getGeometryDoubleExtensionPtr()->getName().empty()) {
        str << "\'" << getGeometryDoubleExtensionPtr()->getName() << "\', ";
    }

    str << val << ") >";


    return str.str();
}

PyObject* GeometryDoubleExtensionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of the python object and the Twin object
    return new GeometryDoubleExtensionPy(new GeometryDoubleExtension);
}

// constructor method
int GeometryDoubleExtensionPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{

    if (PyArg_ParseTuple(args, "")) {
        // default extension
        return 0;
    }

    PyErr_Clear();
    double val;
    if (PyArg_ParseTuple(args, "d", &val)) {
        this->getGeometryDoubleExtensionPtr()->setValue(val);
        return 0;
    }

    PyErr_Clear();
    char* pystr;
    if (PyArg_ParseTuple(args, "ds", &val, &pystr)) {
        this->getGeometryDoubleExtensionPtr()->setValue(val);
        this->getGeometryDoubleExtensionPtr()->setName(pystr);
        return 0;
    }

    PyErr_SetString(
        PyExc_TypeError,
        "GeometryDoubleExtension constructor accepts:\n"
        "-- empty parameter list\n"
        "-- double\n"
        "-- double, string\n"
    );
    return -1;
}

Py::Float GeometryDoubleExtensionPy::getValue() const
{
    return Py::Float(this->getGeometryDoubleExtensionPtr()->getValue());
}

void GeometryDoubleExtensionPy::setValue(Py::Float value)
{
    this->getGeometryDoubleExtensionPtr()->setValue(float(value));
}


PyObject* GeometryDoubleExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeometryDoubleExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
