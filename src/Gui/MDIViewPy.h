#ifndef GUI_MDIVIEWPY_H
#define GUI_MDIVIEWPY_H

#include <Base/PyObjectBase.h>
#include <CXX/Extensions.hxx>
#include <QPointer>
#include <FCGlobal.h>

#include "MDIView.h"

namespace Gui
{
class MDIView;

class GuiExport MDIViewPy: public Py::PythonExtension<MDIViewPy>
{
public:
    static void init_type();  // announce properties and methods
    static PyObject* extension_object_new(PyTypeObject* subtype, PyObject* /*args*/, PyObject* /*kwds*/);

    static Py::Object type();
    static Py::ExtensionObject<MDIViewPy> create(MDIView* mdi);

    explicit MDIViewPy(MDIView* mdi);
    ~MDIViewPy() override;

    Py::Object repr() override;

    /** @name Printing */
    //@{
    Py::Object printView(const Py::Tuple&);
    Py::Object printPdf(const Py::Tuple&);
    Py::Object printPreview(const Py::Tuple&);
    //@}

    /** @name Undo/Redo actions */
    //@{
    Py::Object undoActions(const Py::Tuple&);
    Py::Object redoActions(const Py::Tuple&);
    //@}

    Py::Object sendMessage(const Py::Tuple&);
    Py::Object supportMessage(const Py::Tuple&);
    Py::Object fitAll(const Py::Tuple&);
    Py::Object setActiveObject(const Py::Tuple&);
    Py::Object getActiveObject(const Py::Tuple&);
    Py::Object cast_to_base(const Py::Tuple&);

    MDIView* getMDIViewPtr()
    {
        return _view.data();
    }

private:
    QPointer<MDIView> _view;
};

}  // namespace Gui

#endif  // GUI_MDIVIEWPY_H
