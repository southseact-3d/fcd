// inclusion of the generated files (generated out of MeasureManagerPy.xml)
#include "MeasureManagerPy.h"
#include "MeasureManagerPy.cpp"


using namespace App;


// returns a string which represents the object e.g. when printed in python
std::string MeasureManagerPy::representation() const
{
    return "<App::MeasureManager>";
}

PyObject* MeasureManagerPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int MeasureManagerPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}


PyObject* MeasureManagerPy::addMeasureType(PyObject* args)
{
    PyObject* pyobj = Py_None;
    char *id, *label;

    if (!PyArg_ParseTuple(args, "ssO", &id, &label, &pyobj)) {
        return nullptr;
    }

    MeasureManager::addMeasureType(
        new App::MeasureType {id, label, "", nullptr, nullptr, true, pyobj});

    Py_Return;
}


PyObject* MeasureManagerPy::getMeasureTypes()
{
    Py::List types;
    for (auto& it : MeasureManager::getMeasureTypes()) {
        Py::Tuple type(3);
        type.setItem(0, Py::String(it->identifier));
        type.setItem(1, Py::String(it->label));
        type.setItem(2, Py::Object(it->pythonClass));

        types.append(type);
    }

    return Py::new_reference_to(types);
}
