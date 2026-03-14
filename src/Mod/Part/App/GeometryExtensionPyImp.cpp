#include "GeometryExtension.h"
#include "GeometryExtensionPy.h"
#include "GeometryExtensionPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryExtensionPy::representation() const
{
    return "<GeometryExtension object>";
}

PyObject* GeometryExtensionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(
        PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'GeometryExtension'."
    );
    return nullptr;
}

// constructor method
int GeometryExtensionPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* GeometryExtensionPy::copy(PyObject* args) const
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Part::GeometryExtension* ext = this->getGeometryExtensionPtr();
    PyTypeObject* type = this->GetType();
    PyObject* cpy = nullptr;
    // let the type object decide
    if (type->tp_new) {
        cpy = type->tp_new(type, const_cast<GeometryExtensionPy*>(this), nullptr);
    }
    if (!cpy) {
        PyErr_SetString(PyExc_TypeError, "failed to create copy of the geometry extension");
        return nullptr;
    }

    Part::GeometryExtensionPy* extpy = static_cast<Part::GeometryExtensionPy*>(cpy);
    // the PyMake function must have created the corresponding instance of the 'GeometryExtension'
    // subclass so delete it now to avoid a memory leak
    if (extpy->_pcTwinPointer) {
        Part::GeometryExtension* clone = static_cast<Part::GeometryExtension*>(extpy->_pcTwinPointer);
        delete clone;
    }
    extpy->_pcTwinPointer = ext->copy().release();
    return cpy;
}

Py::String GeometryExtensionPy::getName() const
{
    std::string name = this->getGeometryExtensionPtr()->getName();

    return {name};
}

void GeometryExtensionPy::setName(Py::String arg)
{
    std::string name = arg.as_std_string();

    this->getGeometryExtensionPtr()->setName(name);
}

PyObject* GeometryExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeometryExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
