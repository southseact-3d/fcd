#include <QMetaType>

#include <Base/Quantity.h>
#include <Base/QuantityPy.h>
#include <CXX/Objects.hxx>
#include <Gui/MetaTypes.h>

#include "MaterialFilter.h"

#include "MaterialFilterPy.h"

#include "MaterialFilterPy.cpp"

using namespace Materials;

// Forward declaration
// static PyObject* _pyObjectFromVariant(const QVariant& value);
// static Py::List getList(const QVariant& value);

// returns a string which represents the object e.g. when printed in python
std::string MaterialFilterPy::representation() const
{
    std::stringstream str;
    str << "<MaterialFilter object at " << getMaterialFilterPtr() << ">";

    return str.str();
}

PyObject* MaterialFilterPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    return new MaterialFilterPy(new MaterialFilter());
}

// constructor method
int MaterialFilterPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

Py::String MaterialFilterPy::getName() const
{
    auto filterName = getMaterialFilterPtr()->name();
    return {filterName.toStdString()};
}

void MaterialFilterPy::setName(const Py::String value)
{
    getMaterialFilterPtr()->setName(QString::fromStdString(value));
}

Py::List MaterialFilterPy::getRequiredModels() const
{
    auto listValue = getMaterialFilterPtr()->getRequired();
    Py::List list;

    for (auto& it : *listValue) {
        list.append(Py::String(it.toStdString()));
    }

    return list;
}

void MaterialFilterPy::setRequiredModels(Py::List value)
{
    for (const auto& it : value) {
        Py::String uuid(it);
        getMaterialFilterPtr()->addRequired(QString::fromStdString(uuid));
    }
}

Py::List MaterialFilterPy::getRequiredCompleteModels() const
{
    auto listValue = getMaterialFilterPtr()->getRequiredComplete();
    Py::List list;

    for (auto& it : *listValue) {
        list.append(Py::String(it.toStdString()));
    }

    return list;
}

void MaterialFilterPy::setRequiredCompleteModels(Py::List value)
{
    for (const auto& it : value) {
        Py::String uuid(it);
        getMaterialFilterPtr()->addRequiredComplete(QString::fromStdString(uuid));
    }
}

PyObject* MaterialFilterPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int MaterialFilterPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
