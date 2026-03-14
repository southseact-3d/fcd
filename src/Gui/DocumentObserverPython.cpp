#include <Base/Interpreter.h>

#include "Application.h"
#include "Document.h"
#include "DocumentObserverPython.h"
#include "ViewProvider.h"
#include "ViewProviderDocumentObject.h"


using namespace Gui;
namespace sp = std::placeholders;

std::vector<DocumentObserverPython*> DocumentObserverPython::_instances;

void DocumentObserverPython::addObserver(const Py::Object& obj)
{
    _instances.push_back(new DocumentObserverPython(obj));
}

void DocumentObserverPython::removeObserver(const Py::Object& obj)
{
    DocumentObserverPython* obs = nullptr;
    for (std::vector<DocumentObserverPython*>::iterator it = _instances.begin();
         it != _instances.end();
         ++it) {
        if ((*it)->inst == obj) {
            obs = *it;
            _instances.erase(it);
            break;
        }
    }

    delete obs;
}

DocumentObserverPython::DocumentObserverPython(const Py::Object& obj)
    : inst(obj)
{
    // NOLINTBEGIN
#define FC_PY_ELEMENT_ARG1(_name1, _name2) \
    do { \
        FC_PY_GetCallable(obj.ptr(), "slot" #_name1, py##_name1.py); \
        if (!py##_name1.py.isNone()) \
            py##_name1.slot = Application::Instance->signal##_name2.connect( \
                std::bind(&DocumentObserverPython::slot##_name1, this, sp::_1) \
            ); \
    } while (0);

#define FC_PY_ELEMENT_ARG2(_name1, _name2) \
    do { \
        FC_PY_GetCallable(obj.ptr(), "slot" #_name1, py##_name1.py); \
        if (!py##_name1.py.isNone()) \
            py##_name1.slot = Application::Instance->signal##_name2.connect( \
                std::bind(&DocumentObserverPython::slot##_name1, this, sp::_1, sp::_2) \
            ); \
    } while (0);

    FC_PY_ELEMENT_ARG1(CreatedDocument, NewDocument)
    FC_PY_ELEMENT_ARG1(DeletedDocument, DeleteDocument)
    FC_PY_ELEMENT_ARG1(RelabelDocument, RelabelDocument)
    FC_PY_ELEMENT_ARG1(RenameDocument, RenameDocument)
    FC_PY_ELEMENT_ARG1(ActivateDocument, ActiveDocument)
    FC_PY_ELEMENT_ARG1(CreatedObject, NewObject)
    FC_PY_ELEMENT_ARG1(DeletedObject, DeletedObject)
    FC_PY_ELEMENT_ARG2(BeforeChangeObject, BeforeChangeObject)
    FC_PY_ELEMENT_ARG2(ChangedObject, ChangedObject)
    FC_PY_ELEMENT_ARG1(InEdit, InEdit)
    FC_PY_ELEMENT_ARG1(ResetEdit, ResetEdit)
    // NOLINTEND
}

DocumentObserverPython::~DocumentObserverPython() = default;

void DocumentObserverPython::slotCreatedDocument(const Gui::Document& Doc)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::Document&>(Doc).getPyObject()));
        Base::pyCall(pyCreatedDocument.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotDeletedDocument(const Gui::Document& Doc)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::Document&>(Doc).getPyObject()));
        Base::pyCall(pyDeletedDocument.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotRelabelDocument(const Gui::Document& Doc)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::Document&>(Doc).getPyObject()));
        Base::pyCall(pyRelabelDocument.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotRenameDocument(const Gui::Document& Doc)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::Document&>(Doc).getPyObject()));
        Base::pyCall(pyRenameDocument.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotActivateDocument(const Gui::Document& Doc)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::Document&>(Doc).getPyObject()));
        Base::pyCall(pyActivateDocument.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotCreatedObject(const Gui::ViewProvider& Obj)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProvider&>(Obj).getPyObject()));
        Base::pyCall(pyCreatedObject.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotDeletedObject(const Gui::ViewProvider& Obj)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProvider&>(Obj).getPyObject()));
        Base::pyCall(pyDeletedObject.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotBeforeChangeObject(
    const Gui::ViewProvider& Obj,
    const App::Property& Prop
)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(2);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProvider&>(Obj).getPyObject()));
        // If a property is touched but not part of a document object then its name is null.
        // In this case the slot function must not be called.
        const char* prop_name = Obj.getPropertyName(&Prop);
        if (prop_name) {
            args.setItem(1, Py::String(prop_name));
            Base::pyCall(pyBeforeChangeObject.ptr(), args.ptr());
        }
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotChangedObject(const Gui::ViewProvider& Obj, const App::Property& Prop)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(2);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProvider&>(Obj).getPyObject()));
        // If a property is touched but not part of a document object then its name is null.
        // In this case the slot function must not be called.
        const char* prop_name = Obj.getPropertyName(&Prop);
        if (prop_name) {
            args.setItem(1, Py::String(prop_name));
            Base::pyCall(pyChangedObject.ptr(), args.ptr());
        }
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotInEdit(const Gui::ViewProviderDocumentObject& Obj)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProviderDocumentObject&>(Obj).getPyObject()));
        Base::pyCall(pyInEdit.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}

void DocumentObserverPython::slotResetEdit(const Gui::ViewProviderDocumentObject& Obj)
{
    Base::PyGILStateLocker lock;
    try {
        Py::Tuple args(1);
        args.setItem(0, Py::asObject(const_cast<Gui::ViewProviderDocumentObject&>(Obj).getPyObject()));
        Base::pyCall(pyResetEdit.ptr(), args.ptr());
    }
    catch (Py::Exception&) {
        Base::PyException e;  // extract the Python error text
        e.reportException();
    }
}
