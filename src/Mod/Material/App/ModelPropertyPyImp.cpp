#include "Model.h"
#include "ModelPropertyPy.h"

#include "ModelPropertyPy.cpp"

using namespace Materials;

// returns a string which represents the object e.g. when printed in python
std::string ModelPropertyPy::representation() const
{
    std::stringstream str;
    str << "<ModelProperty object at " << getModelPropertyPtr() << ">";

    return str.str();
}

PyObject* ModelPropertyPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    return new ModelPropertyPy(new ModelProperty());
}

// constructor method
int ModelPropertyPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

Py::String ModelPropertyPy::getName() const
{
    return Py::String(getModelPropertyPtr()->getName().toStdString());
}

void ModelPropertyPy::setName(Py::String arg)
{
    getModelPropertyPtr()->setName(QString::fromStdString(arg));
}

Py::String ModelPropertyPy::getDisplayName() const
{
    return Py::String(getModelPropertyPtr()->getDisplayName().toStdString());
}

void ModelPropertyPy::setDisplayName(Py::String arg)
{
    getModelPropertyPtr()->setDisplayName(QString::fromStdString(arg));
}

Py::String ModelPropertyPy::getType() const
{
    return Py::String(getModelPropertyPtr()->getPropertyType().toStdString());
}

void ModelPropertyPy::setType(Py::String arg)
{
    getModelPropertyPtr()->setPropertyType(QString::fromStdString(arg));
}

Py::String ModelPropertyPy::getUnits() const
{
    return Py::String(getModelPropertyPtr()->getUnits().toStdString());
}

void ModelPropertyPy::setUnits(Py::String arg)
{
    getModelPropertyPtr()->setUnits(QString::fromStdString(arg));
}

Py::String ModelPropertyPy::getURL() const
{
    return Py::String(getModelPropertyPtr()->getURL().toStdString());
}

void ModelPropertyPy::setURL(Py::String arg)
{
    getModelPropertyPtr()->setURL(QString::fromStdString(arg));
}

Py::String ModelPropertyPy::getDescription() const
{
    return Py::String(getModelPropertyPtr()->getDescription().toStdString());
}

void ModelPropertyPy::setDescription(Py::String arg)
{
    getModelPropertyPtr()->setDescription(QString::fromStdString(arg));
}

Py::List ModelPropertyPy::getColumns() const
{
    Py::List list;

    auto columns = getModelPropertyPtr()->getColumns();
    for (auto& column : columns) {
        PyObject* modelPropertyPy = new ModelPropertyPy(new ModelProperty(column));
        list.append(Py::Object(modelPropertyPy, true));
    }

    return list;
}

Py::String ModelPropertyPy::getInheritance() const
{
    return Py::String(getModelPropertyPtr()->getInheritance().toStdString());
}

Py::Boolean ModelPropertyPy::getInherited() const
{
    return getModelPropertyPtr()->isInherited();
}

PyObject* ModelPropertyPy::addColumn(PyObject* args)
{
    PyObject* object;
    if (!PyArg_ParseTuple(args, "O!", &ModelPropertyPy::Type, &object)) {
        return nullptr;
    }
    ModelProperty* property = static_cast<ModelPropertyPy*>(object)->getModelPropertyPtr();

    getModelPropertyPtr()->addColumn(*property);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* ModelPropertyPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ModelPropertyPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}