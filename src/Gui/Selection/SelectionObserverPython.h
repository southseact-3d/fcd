#ifndef GUI_SELECTIONOBSERVERPYTHON_H
#define GUI_SELECTIONOBSERVERPYTHON_H

#include <CXX/Objects.hxx>
#include "Selection.h"


namespace Gui
{

class GuiExport SelectionObserverPythonHandler
{

public:
    /// Constructor
    explicit SelectionObserverPythonHandler() = default;
    virtual ~SelectionObserverPythonHandler();

    void init(PyObject* obj);
    void handleSelectionChanged(const SelectionChanges& msg);

protected:
    void addSelection(const SelectionChanges&);
    void removeSelection(const SelectionChanges&);
    void setSelection(const SelectionChanges&);
    void clearSelection(const SelectionChanges&);
    void setPreselection(const SelectionChanges&);
    void removePreselection(const SelectionChanges&);
    void pickedListChanged();

private:
#define FC_PY_SEL_OBSERVER \
    FC_PY_ELEMENT(onSelectionChanged) \
    FC_PY_ELEMENT(addSelection) \
    FC_PY_ELEMENT(removeSelection) \
    FC_PY_ELEMENT(setSelection) \
    FC_PY_ELEMENT(clearSelection) \
    FC_PY_ELEMENT(setPreselection) \
    FC_PY_ELEMENT(removePreselection) \
    FC_PY_ELEMENT(pickedListChanged)

#undef FC_PY_ELEMENT
#define FC_PY_ELEMENT(_name) Py::Object py_##_name;

    FC_PY_SEL_OBSERVER
};

/**
 * The SelectionObserverPython class implements a mechanism to register
 * a Python class instance implementing the required interface in order
 * to be notified on selection changes.
 *
 * @author Werner Mayer
 */
class GuiExport SelectionObserverPython: public SelectionObserverPythonHandler,
                                         public SelectionObserver
{

public:
    /// Constructor
    explicit SelectionObserverPython(
        const Py::Object& obj,
        ResolveMode resolve = ResolveMode::OldStyleElement
    );
    ~SelectionObserverPython() override;

    static void addObserver(const Py::Object& obj, ResolveMode resolve = ResolveMode::OldStyleElement);
    static void removeObserver(const Py::Object& obj);

private:
    void onSelectionChanged(const SelectionChanges& msg) override;

    Py::Object inst;
    static std::vector<SelectionObserverPython*> _instances;
};

}  // namespace Gui

#endif  // GUI_SELECTIONOBSERVERPYTHON_H
