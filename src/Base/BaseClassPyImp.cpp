#include "Type.h"

// generated out of BaseClass.pyi
#include "BaseClassPy.h"
#include "BaseClassPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
std::string BaseClassPy::representation() const
{
    return {"<binding object>"};
}

PyObject* BaseClassPy::isDerivedFrom(PyObject* args) const
{
    char* name {};
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }

    auto type = Type::fromName(name);
    bool valid = (!type.isBad() && getBaseClassPtr()->isDerivedFrom(type));
    return PyBool_FromLong(valid ? 1 : 0);
}

PyObject* BaseClassPy::getAllDerivedFrom(PyObject* args) const
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    std::vector<Base::Type> ary;
    Type::getAllDerivedFrom(getBaseClassPtr()->getTypeId(), ary);
    Py::List res;
    for (const auto& it : ary) {
        res.append(Py::String(it.getName()));
    }
    return Py::new_reference_to(res);
}

Py::String BaseClassPy::getTypeId() const
{
    return {std::string(getBaseClassPtr()->getTypeId().getName())};
}

Py::String BaseClassPy::getModule() const
{
    std::string module(getBaseClassPtr()->getTypeId().getName());
    std::string::size_type pos = module.find_first_of("::");

    if (pos != std::string::npos) {
        module = std::string(module, 0, pos);
    }
    else {
        module.clear();
    }

    return {module};
}

PyObject* BaseClassPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int BaseClassPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
