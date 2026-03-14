#ifndef APP_DOCUMENTOBSERVERPYTHON_H
#define APP_DOCUMENTOBSERVERPYTHON_H

#include <FCGlobal.h>
#include <fastsignals/signal.h>
#include <CXX/Objects.hxx>
#include <string>
#include <vector>

namespace App
{

class Document;
class DocumentObject;
class ExtensionContainer;
class Property;

/**
 * The DocumentObserverPython class is used to notify registered Python instances
 * whenever something happens to a document, like creation, destruction, adding or
 * removing objects or when property changes.
 *
 * @author Werner Mayer
 */
class AppExport DocumentObserverPython
{

public:
    /// Constructor
    DocumentObserverPython(const Py::Object& obj);
    virtual ~DocumentObserverPython();

    static void addObserver(const Py::Object& obj);
    static void removeObserver(const Py::Object& obj);

private:
    /** Checks if a new document was created */
    void slotCreatedDocument(const App::Document& Doc);
    /** Checks if the given document is about to be closed */
    void slotDeletedDocument(const App::Document& Doc);
    /** Checks if the given document is relabeled */
    void slotRelabelDocument(const App::Document& Doc);
    /** Checks if the given document is activated */
    void slotActivateDocument(const App::Document& Doc);
    /** The property of an observed document has changed */
    void slotBeforeChangeDocument(const App::Document& Obj, const App::Property& Prop);
    /** The property of an observed document has changed */
    void slotChangedDocument(const App::Document& Obj, const App::Property& Prop);
    /** Checks if a new object was added. */
    void slotCreatedObject(const App::DocumentObject& Obj);
    /** Checks if the given object is about to be removed. */
    void slotDeletedObject(const App::DocumentObject& Obj);
    /** The property of an observed object has changed */
    void slotBeforeChangeObject(const App::DocumentObject& Obj, const App::Property& Prop);
    /** The property of an observed object has changed */
    void slotChangedObject(const App::DocumentObject& Obj, const App::Property& Prop);
    /** Undoes the last transaction of the document */
    void slotUndoDocument(const App::Document& Doc);
    /** Redoes the last undone transaction of the document */
    void slotRedoDocument(const App::Document& Doc);
    /** Called when a given object is recomputed */
    void slotRecomputedObject(const App::DocumentObject& Obj);
    /** Called before an observed document is recomputed */
    void slotBeforeRecomputeDocument(const App::Document& Doc);
    /** Called when an observed document is recomputed */
    void slotRecomputedDocument(const App::Document& Doc);
    /** Called when an observed document opens a transaction */
    void slotOpenTransaction(const App::Document& Doc, std::string str);
    /** Called when an observed document commits a transaction */
    void slotCommitTransaction(const App::Document& Doc);
    /** Called when an observed document aborts a transaction */
    void slotAbortTransaction(const App::Document& Doc);
    /** Called after application wide undo */
    void slotUndo();
    /** Called after application wide redo */
    void slotRedo();
    /** Called before closing/aborting application active transaction */
    void slotBeforeCloseTransaction(bool abort);
    /** Called after closing/aborting application active transaction */
    void slotCloseTransaction(bool abort);
    /** Called when an object gets a new dynamic property added*/
    void slotAppendDynamicProperty(const App::Property& Prop);
    /** Called when an object gets a dynamic property removed*/
    void slotRemoveDynamicProperty(const App::Property& Prop);
    /** Called when an object property gets a new editor relevant status like hidden or read only*/
    void slotChangePropertyEditor(const App::Document& Doc, const App::Property& Prop);
    /** Called when a document is about to be saved*/
    void slotStartSaveDocument(const App::Document&, const std::string&);
    /** Called when an document has been saved*/
    void slotFinishSaveDocument(const App::Document&, const std::string&);
    /** Called before an object gets a new extension added*/
    void slotBeforeAddingDynamicExtension(const App::ExtensionContainer&, std::string extension);
    /** Called when an object gets a dynamic extension added*/
    void slotAddedDynamicExtension(const App::ExtensionContainer&, std::string extension);


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
    Connection pyActivateDocument;
    Connection pyUndoDocument;
    Connection pyRedoDocument;
    Connection pyBeforeChangeDocument;
    Connection pyChangedDocument;
    Connection pyCreatedObject;
    Connection pyDeletedObject;
    Connection pyBeforeChangeObject;
    Connection pyChangedObject;
    Connection pyRecomputedObject;
    Connection pyBeforeRecomputeDocument;
    Connection pyRecomputedDocument;
    Connection pyOpenTransaction;
    Connection pyCommitTransaction;
    Connection pyAbortTransaction;
    Connection pyUndo;
    Connection pyRedo;
    Connection pyBeforeCloseTransaction;
    Connection pyCloseTransaction;
    Connection pyStartSaveDocument;
    Connection pyFinishSaveDocument;
    Connection pyAppendDynamicProperty;
    Connection pyRemoveDynamicProperty;
    Connection pyChangePropertyEditor;
    Connection pyBeforeAddingDynamicExtension;
    Connection pyAddedDynamicExtension;
};

}  // namespace App

#endif  // APP_DOCUMENTOBSERVERPYTHON_H
