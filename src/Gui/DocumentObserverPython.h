#ifndef GUI_DOCUMENTOBSERVERPYTHON_H
#define GUI_DOCUMENTOBSERVERPYTHON_H

#include <FCGlobal.h>
#include <fastsignals/signal.h>
#include <CXX/Objects.hxx>
#include <string>
#include <vector>

namespace App
{
class Property;
}

namespace Gui
{

class Document;
class ViewProvider;
class ViewProviderDocumentObject;

/**
 * The DocumentObserverPython class is used to notify registered Python instances
 * whenever something happens to a document, like creation, destruction, adding or
 * removing viewproviders or when viewprovider property changes. This is the equivalent to the app
 * python document observer
 */
class GuiExport DocumentObserverPython
{

public:
    /// Constructor
    DocumentObserverPython(const Py::Object& obj);
    virtual ~DocumentObserverPython();

    static void addObserver(const Py::Object& obj);
    static void removeObserver(const Py::Object& obj);

private:
    /** Checks if a new document was created */
    void slotCreatedDocument(const Gui::Document& Doc);
    /** Checks if the given document is about to be closed */
    void slotDeletedDocument(const Gui::Document& Doc);
    /** Checks if the given document is relabeled */
    void slotRelabelDocument(const Gui::Document& Doc);
    /** Checks if the given document is renamed */
    void slotRenameDocument(const Gui::Document& Doc);
    /** Checks if the given document is activated */
    void slotActivateDocument(const Gui::Document& Doc);
    /** Checks if a new object was added. */
    void slotCreatedObject(const Gui::ViewProvider& Obj);
    /** Checks if the given object is about to be removed. */
    void slotDeletedObject(const Gui::ViewProvider& Obj);
    /** The property of an observed object has changed */
    void slotBeforeChangeObject(const Gui::ViewProvider& Obj, const App::Property& Prop);
    /** The property of an observed object has changed */
    void slotChangedObject(const Gui::ViewProvider& Obj, const App::Property& Prop);
    /** The object was set into edit mode */
    void slotInEdit(const Gui::ViewProviderDocumentObject& Obj);
    /** The has left edit mode */
    void slotResetEdit(const Gui::ViewProviderDocumentObject& Obj);

private:
    Py::Object inst;
    static std::vector<DocumentObserverPython*> _instances;

    using Connection = struct PythonObject
    {
        fastsignals::scoped_connection slot;
        Py::Object py;
        PyObject* ptr()
        {
            return py.ptr();
        }
    };

    Connection pyCreatedDocument;
    Connection pyDeletedDocument;
    Connection pyRelabelDocument;
    Connection pyRenameDocument;
    Connection pyActivateDocument;
    Connection pyCreatedObject;
    Connection pyDeletedObject;
    Connection pyBeforeChangeObject;
    Connection pyChangedObject;
    Connection pyInEdit;
    Connection pyResetEdit;
};

}  // namespace Gui

#endif  // GUI_DOCUMENTOBSERVERPYTHON_H
