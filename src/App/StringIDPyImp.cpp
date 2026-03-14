#include "StringHasher.h"

#include "StringIDPy.h"
#include "StringIDPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string StringIDPy::representation() const
{
    return getStringIDPtr()->toString(this->_index);
}

PyObject* StringIDPy::isSame(PyObject* args) const
{
    PyObject* other = nullptr;
    if (!PyArg_ParseTuple(args, "O!", &StringIDPy::Type, &other)) {
        return nullptr;
    }

    auto* otherPy = static_cast<StringIDPy*>(other);
    bool same =
        (otherPy->getStringIDPtr() == this->getStringIDPtr()) && (otherPy->_index == this->_index);

    return PyBool_FromLong(same ? 1 : 0);
}

Py::Long StringIDPy::getValue() const
{
    return Py::Long(getStringIDPtr()->value());
}

Py::List StringIDPy::getRelated() const
{
    Py::List list;
    for (const auto& id : getStringIDPtr()->relatedIDs()) {
        list.append(Py::Long(id.value()));
    }

    return list;
}

Py::String StringIDPy::getData() const
{
    return {getStringIDPtr()->dataToText(this->_index)};
}

Py::Boolean StringIDPy::getIsBinary() const
{
    return {getStringIDPtr()->isBinary()};
}

Py::Boolean StringIDPy::getIsHashed() const
{
    return {getStringIDPtr()->isHashed()};
}

Py::Long StringIDPy::getIndex() const
{
    return Py::Long(this->_index);
}

void StringIDPy::setIndex(Py::Long index)
{
    this->_index = index;
}

PyObject* StringIDPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int StringIDPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
