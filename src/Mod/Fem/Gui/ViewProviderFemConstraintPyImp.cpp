#include <Inventor/nodes/SoSeparator.h>


#include <Base/Interpreter.h>

#include "ViewProviderFemConstraintPy.h"
#include "ViewProviderFemConstraintPy.cpp"


using namespace FemGui;

// returns a string which represent the object e.g. when printed in python
std::string ViewProviderFemConstraintPy::representation() const
{
    std::stringstream str;
    str << "<View provider FemConstraint object at " << getViewProviderFemConstraintPtr() << ">";

    return str.str();
}

PyObject* ViewProviderFemConstraintPy::loadSymbol(PyObject* args)
{
    const char* name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }

    getViewProviderFemConstraintPtr()->loadSymbol(name);

    Py_Return;
}

Py::Object ViewProviderFemConstraintPy::getSymbolNode() const
{
    try {
        SoSeparator* sep = getViewProviderFemConstraintPtr()->getSymbolSeparator();
        if (sep) {
            PyObject* Ptr
                = Base::Interpreter().createSWIGPointerObj("pivy.coin", "_p_SoSeparator", sep, 1);
            sep->ref();

            return Py::Object(Ptr, true);
        }
        else {
            return Py::None();
        }
    }
    catch (const Base::Exception& e) {
        throw Py::RuntimeError(e.what());
    }
}

Py::Object ViewProviderFemConstraintPy::getExtraSymbolNode() const
{
    try {
        SoSeparator* sep = getViewProviderFemConstraintPtr()->getExtraSymbolSeparator();
        if (sep) {
            PyObject* Ptr
                = Base::Interpreter().createSWIGPointerObj("pivy.coin", "_p_SoSeparator", sep, 1);
            sep->ref();

            return Py::Object(Ptr, true);
        }
        else {
            return Py::None();
        }
    }
    catch (const Base::Exception& e) {
        throw Py::RuntimeError(e.what());
    }
}

Py::Boolean ViewProviderFemConstraintPy::getRotateSymbol() const
{
    return Py::Boolean(getViewProviderFemConstraintPtr()->getRotateSymbol());
}

void ViewProviderFemConstraintPy::setRotateSymbol(Py::Boolean arg)
{
    getViewProviderFemConstraintPtr()->setRotateSymbol((arg));
}

PyObject* ViewProviderFemConstraintPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderFemConstraintPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
