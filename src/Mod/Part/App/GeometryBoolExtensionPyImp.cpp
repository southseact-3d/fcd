#include "GeometryBoolExtensionPy.h"
#include "GeometryBoolExtensionPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryBoolExtensionPy::representation() const
{
    std::stringstream str;
    bool val = getGeometryBoolExtensionPtr()->getValue();
    str << "<GeometryBoolExtension (";

    if (!getGeometryBoolExtensionPtr()->getName().empty()) {
        str << "\'" << getGeometryBoolExtensionPtr()->getName() << "\', ";
    }

    str << (val ? "True" : "False") << ") >";


    return str.str();
}

PyObject* GeometryBoolExtensionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of the python object and the Twin object
    return new GeometryBoolExtensionPy(new GeometryBoolExtension);
}

// constructor method
int GeometryBoolExtensionPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{

    if (PyArg_ParseTuple(args, "")) {
        // default extension
        return 0;
    }

    PyErr_Clear();
    PyObject* val;
    if (PyArg_ParseTuple(args, "O!", &PyBool_Type, &val)) {
        this->getGeometryBoolExtensionPtr()->setValue(Base::asBoolean(val));
        return 0;
    }

    PyErr_Clear();
    char* pystr;
    if (PyArg_ParseTuple(args, "O!s", &PyBool_Type, &val, &pystr)) {
        this->getGeometryBoolExtensionPtr()->setValue(Base::asBoolean(val));
        this->getGeometryBoolExtensionPtr()->setName(pystr);
        return 0;
    }

    PyErr_SetString(
        PyExc_TypeError,
        "GeometryBoolExtension constructor accepts:\n"
        "-- empty parameter list\n"
        "-- Boolean\n"
        "-- Boolean, string\n"
    );
    return -1;
}

Py::Boolean GeometryBoolExtensionPy::getValue() const
{
    return {this->getGeometryBoolExtensionPtr()->getValue()};
}

void GeometryBoolExtensionPy::setValue(Py::Boolean value)
{
    this->getGeometryBoolExtensionPtr()->setValue(value);
}

PyObject* GeometryBoolExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeometryBoolExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
