#ifndef GUI_VIEW3DVIEWERPY_H
#define GUI_VIEW3DVIEWERPY_H

#include <CXX/Extensions.hxx>
#include <list>


namespace Gui
{

class View3DInventorViewer;

/**
 * @brief Python interface for View3DInventorViewer
 *
 * The interface does not offer all methods the c++ View3DInventorViewer counterpart has,
 * respectively also not everything the QuarterWidget and the SoQtQuarterAdaptor offers. It only
 * exposes methods with additional functionality in comparison to the View3DInventorPy class.
 * Everything that can be done from there has no interface here.
 */
class View3DInventorViewerPy: public Py::PythonExtension<View3DInventorViewerPy>
{
public:
    static void init_type();  // announce properties and methods

    explicit View3DInventorViewerPy(View3DInventorViewer* vi);
    ~View3DInventorViewerPy() override;

    Py::Object repr() override;
    Py::Object getattr(const char*) override;
    int setattr(const char*, const Py::Object&) override;

    // exposed methods
    Py::Object getSoEventManager(const Py::Tuple&);
    Py::Object getSoRenderManager(const Py::Tuple&);
    Py::Object getSceneGraph(const Py::Tuple&);
    Py::Object setSceneGraph(const Py::Tuple&);

    Py::Object seekToPoint(const Py::Tuple&);
    Py::Object setFocalDistance(const Py::Tuple& args);
    Py::Object getFocalDistance(const Py::Tuple& args);
    Py::Object getPointOnFocalPlane(const Py::Tuple& args);
    Py::Object getPickRadius(const Py::Tuple& args);
    Py::Object setPickRadius(const Py::Tuple& args);

    Py::Object setupEditingRoot(const Py::Tuple& args);
    Py::Object resetEditingRoot(const Py::Tuple& args);

    Py::Object setGradientBackground(const Py::Tuple& args);
    Py::Object setGradientBackgroundColor(const Py::Tuple& args);
    Py::Object setBackgroundColor(const Py::Tuple& args);
    Py::Object setRedirectToSceneGraph(const Py::Tuple& args);
    Py::Object isRedirectedToSceneGraph(const Py::Tuple& args);
    Py::Object grabFramebuffer(const Py::Tuple& args);

    Py::Object setOverrideMode(const Py::Tuple& args);

    // NaviCube handling
    Py::Object setEnabledNaviCube(const Py::Tuple& args);
    Py::Object isEnabledNaviCube(const Py::Tuple& args);
    Py::Object setNaviCubeCorner(const Py::Tuple& args);

    Py::Object getNavigationStyle(const Py::Tuple&);

private:
    using method_varargs_handler = PyObject* (*)(PyObject * _self, PyObject* _args);
    static method_varargs_handler pycxx_handler;
    static PyObject* method_varargs_ext_handler(PyObject* _self, PyObject* _args);

private:
    std::list<PyObject*> callbacks;
    View3DInventorViewer* _viewer;
    friend class View3DInventorViewer;
};

}  // namespace Gui

#endif  // GUI_VIEW3DPY_H
