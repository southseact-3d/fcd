// inclusion of the generated files (generated out of FeaturePathCompoundPy.xml)
#include "FeaturePathCompoundPy.h"
#include "FeaturePathCompoundPy.cpp"


using namespace Path;

// returns a string which represents the object e.g. when printed in python
std::string FeaturePathCompoundPy::representation() const
{
    return std::string("<Path::FeatureCompound>");
}


PyObject* FeaturePathCompoundPy::addObject(PyObject* args)
{
    PyObject* object;
    if (!PyArg_ParseTuple(args, "O!", &(DocumentObjectPy::Type), &object)) {
        return nullptr;
    }

    DocumentObjectPy* docObj = static_cast<DocumentObjectPy*>(object);
    if (!docObj->getDocumentObjectPtr() || !docObj->getDocumentObjectPtr()->isAttachedToDocument()) {
        PyErr_SetString(Base::PyExc_FC_GeneralError, "Cannot add an invalid object");
        return nullptr;
    }
    if (docObj->getDocumentObjectPtr()->getDocument() != getFeaturePathCompoundPtr()->getDocument()) {
        PyErr_SetString(
            Base::PyExc_FC_GeneralError,
            "Cannot add an object from another document to this group"
        );
        return nullptr;
    }
    if (docObj->getDocumentObjectPtr() == this->getFeaturePathCompoundPtr()) {
        PyErr_SetString(Base::PyExc_FC_GeneralError, "Cannot add a group object to itself");
        return nullptr;
    }

    FeatureCompound* comp = getFeaturePathCompoundPtr();

    if (comp->isDerivedFrom<Path::FeatureCompoundPython>()) {
        FeatureCompoundPython* comppy = static_cast<FeatureCompoundPython*>(comp);
        App::Property* proxy = comppy->getPropertyByName("Proxy");
        if (proxy && proxy->is<App::PropertyPythonObject>()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("addObject"))) {
                Py::Callable method(vp.getAttr(std::string("addObject")));
                // check to which method this belongs to avoid an infinite recursion
                if (method.getAttr(std::string("__self__")) != Py::Object(this)) {
                    Py::Tuple arg(1);
                    arg[0] = Py::Object(object);
                    method.apply(arg);
                    Py_Return;
                }
            }
        }
    }

    comp->addObject(docObj->getDocumentObjectPtr());
    Py_Return;
}


PyObject* FeaturePathCompoundPy::removeObject(PyObject* args)
{
    PyObject* object;
    if (!PyArg_ParseTuple(args, "O!", &(DocumentObjectPy::Type), &object)) {
        return nullptr;
    }

    DocumentObjectPy* docObj = static_cast<DocumentObjectPy*>(object);
    if (!docObj->getDocumentObjectPtr() || !docObj->getDocumentObjectPtr()->isAttachedToDocument()) {
        PyErr_SetString(Base::PyExc_FC_GeneralError, "Cannot remove an invalid object");
        return nullptr;
    }
    if (docObj->getDocumentObjectPtr()->getDocument() != getFeaturePathCompoundPtr()->getDocument()) {
        PyErr_SetString(
            Base::PyExc_FC_GeneralError,
            "Cannot remove an object from another document from this group"
        );
        return nullptr;
    }

    FeatureCompound* comp = getFeaturePathCompoundPtr();

    if (comp->isDerivedFrom<Path::FeatureCompoundPython>()) {
        FeatureCompoundPython* comppy = static_cast<FeatureCompoundPython*>(comp);
        App::Property* proxy = comppy->getPropertyByName("Proxy");
        if (proxy && proxy->is<App::PropertyPythonObject>()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("removeObject"))) {
                Py::Callable method(vp.getAttr(std::string("removeObject")));
                // check to which method this belongs to avoid an infinite recursion
                if (method.getAttr(std::string("__self__")) != Py::Object(this)) {
                    Py::Tuple arg(1);
                    arg[0] = Py::Object(object);
                    method.apply(arg);
                    Py_Return;
                }
            }
        }
    }

    comp->removeObject(docObj->getDocumentObjectPtr());
    Py_Return;
}


PyObject* FeaturePathCompoundPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}


int FeaturePathCompoundPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
