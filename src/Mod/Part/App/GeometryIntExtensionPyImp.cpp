#include "GeometryIntExtensionPy.h"
#include "GeometryIntExtensionPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryIntExtensionPy::representation() const
{
    std::stringstream str;
    long val = getGeometryIntExtensionPtr()->getValue();
    str << "<GeometryIntExtension (";

    if (!getGeometryIntExtensionPtr()->getName().empty()) {
        str << "\'" << getGeometryIntExtensionPtr()->getName() << "\', ";
    }

    str << val << ") >";


    return str.str();
}

PyObject* GeometryIntExtensionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of the python object and the Twin object
    return new GeometryIntExtensionPy(new GeometryIntExtension);
}

// constructor method
int GeometryIntExtensionPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{

    if (PyArg_ParseTuple(args, "")) {
        // default extension
        return 0;
    }

    PyErr_Clear();
    long val;
    if (PyArg_ParseTuple(args, "l", &val)) {
        this->getGeometryIntExtensionPtr()->setValue(val);
        return 0;
    }

    PyErr_Clear();
    char* pystr;
    if (PyArg_ParseTuple(args, "ls", &val, &pystr)) {
        this->getGeometryIntExtensionPtr()->setValue(val);
        this->getGeometryIntExtensionPtr()->setName(pystr);
        return 0;
    }

    PyErr_SetString(
        PyExc_TypeError,
        "GeometryIntExtension constructor accepts:\n"
        "-- empty parameter list\n"
        "-- long int\n"
        "-- long int, string\n"
    );
    return -1;
}

Py::Long GeometryIntExtensionPy::getValue() const
{
    return Py::Long(this->getGeometryIntExtensionPtr()->getValue());
}

void GeometryIntExtensionPy::setValue(Py::Long value)
{
    this->getGeometryIntExtensionPtr()->setValue(long(value));
}


PyObject* GeometryIntExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeometryIntExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
