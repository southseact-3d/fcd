#include <sstream>

#include "ViewProviderDocumentObject.h"

// generated out of ViewProviderExtension.pyi
#include "ViewProviderExtensionPy.h"
#include "ViewProviderExtensionPy.cpp"

using namespace Gui;

// returns a string which represent the object e.g. when printed in python
std::string ViewProviderExtensionPy::representation() const
{
    return {"<view provider extension>"};
}

PyObject* ViewProviderExtensionPy::setIgnoreOverlayIcon(PyObject* args)
{
    PyObject* ignore;
    const char* name = nullptr;
    if (!PyArg_ParseTuple(args, "O!s", &PyBool_Type, &ignore, &name)) {
        return nullptr;
    }

    ViewProviderExtension* ext = getViewProviderExtensionPtr();
    if (name) {
        Base::Type type = Base::Type::fromName(name);
        ext = dynamic_cast<ViewProviderExtension*>(
            ext->getExtendedContainer()->getExtension(type, true, true)
        );
        if (!ext) {
            PyErr_SetString(PyExc_NameError, "no such extension");
            return nullptr;
        }
    }

    ext->setIgnoreOverlayIcon(Base::asBoolean(ignore));
    Py_Return;
}

PyObject* ViewProviderExtensionPy::ignoreOverlayIcon(PyObject* args) const
{
    const char* name = nullptr;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }

    ViewProviderExtension* ext = getViewProviderExtensionPtr();
    if (name) {
        Base::Type type = Base::Type::fromName(name);
        ext = dynamic_cast<ViewProviderExtension*>(
            ext->getExtendedContainer()->getExtension(type, true, true)
        );
        if (!ext) {
            PyErr_SetString(PyExc_NameError, "no such extension");
            return nullptr;
        }
    }

    bool ignore = ext->ignoreOverlayIcon();
    return Py_BuildValue("O", (ignore ? Py_True : Py_False));
}

PyObject* ViewProviderExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
