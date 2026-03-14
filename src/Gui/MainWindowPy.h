#ifndef GUI_MAINWINDOWPY_H
#define GUI_MAINWINDOWPY_H

#include <Base/PyObjectBase.h>
#include <CXX/Extensions.hxx>
#include <QPointer>
#include <FCGlobal.h>

namespace Gui
{
class MainWindow;

class GuiExport MainWindowPy: public Py::PythonExtension<MainWindowPy>
{
public:
    static void init_type();
    static PyObject* extension_object_new(PyTypeObject* subtype, PyObject* /*args*/, PyObject* /*kwds*/);

    static Py::Object createWrapper(MainWindow* mw);
    static Py::Object type();
    static Py::ExtensionObject<MainWindowPy> create(MainWindow* mw);

    explicit MainWindowPy(MainWindow* mw);
    ~MainWindowPy() override;

    Py::Object repr() override;

    Py::Object getWindows(const Py::Tuple&);
    Py::Object getWindowsOfType(const Py::Tuple&);
    Py::Object setActiveWindow(const Py::Tuple&);
    Py::Object getActiveWindow(const Py::Tuple&);
    Py::Object addWindow(const Py::Tuple&);
    Py::Object removeWindow(const Py::Tuple&);

    Py::Object showHint(const Py::Tuple&);
    Py::Object hideHint(const Py::Tuple&);

private:
    QPointer<MainWindow> _mw;
};

}  // namespace Gui

#endif  // GUI_MAINWINDOWPY_H
