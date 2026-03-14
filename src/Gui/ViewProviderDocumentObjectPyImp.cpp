#include <sstream>

#include <App/DocumentObjectPy.h>

#include "Document.h"

// generated out of ViewProviderDocumentObject.pyi
#include "ViewProviderDocumentObjectPy.h"
#include "ViewProviderDocumentObjectPy.cpp"

using namespace Gui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderDocumentObjectPy::representation() const
{
    std::stringstream str;
    str << "<View provider object at " << getViewProviderDocumentObjectPtr() << ">";

    return str.str();
}

PyObject* ViewProviderDocumentObjectPy::update(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    PY_TRY
    {
        getViewProviderDocumentObjectPtr()->updateView();
        Py_Return;
    }
    PY_CATCH;
}

Py::Object ViewProviderDocumentObjectPy::getObject() const
{
    App::DocumentObject* obj = getViewProviderDocumentObjectPtr()->getObject();
    return Py::Object(obj->getPyObject(), true);  // do not inc'ref twice
}

void ViewProviderDocumentObjectPy::setObject(Py::Object pyobj)
{
    if (!PyObject_TypeCheck(*pyobj, &App::DocumentObjectPy::Type)) {
        throw Py::TypeError("Expect document object");
    }
    App::DocumentObject* obj = getViewProviderDocumentObjectPtr()->getObject();
    if (obj) {
        throw Py::RuntimeError("View object already attached");
    }
    getViewProviderDocumentObjectPtr()->attach(
        static_cast<App::DocumentObjectPy*>(*pyobj)->getDocumentObjectPtr()
    );
}

Py::Boolean ViewProviderDocumentObjectPy::getForceUpdate() const
{
    return {getViewProviderDocumentObjectPtr()->isUpdateForced()};
}

void ViewProviderDocumentObjectPy::setForceUpdate(Py::Boolean arg)
{
    getViewProviderDocumentObjectPtr()->forceUpdate(arg);
}

Py::Object ViewProviderDocumentObjectPy::getDocument() const
{
    Document* doc = getViewProviderDocumentObjectPtr()->getDocument();
    return Py::asObject(doc->getPyObject());  // do not inc'ref twice
}

PyObject* ViewProviderDocumentObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderDocumentObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
