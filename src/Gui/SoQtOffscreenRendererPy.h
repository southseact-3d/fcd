#ifndef GUI_SOQTOFFSCREENRENDERERPY_H
#define GUI_SOQTOFFSCREENRENDERERPY_H

#include <CXX/Extensions.hxx>

#include "SoFCOffscreenRenderer.h"


namespace Gui
{

class SoQtOffscreenRendererPy: public Py::PythonClass<SoQtOffscreenRendererPy>
{
public:
    static void init_type();

    SoQtOffscreenRendererPy(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
    ~SoQtOffscreenRendererPy() override;

    Py::Object repr() override;

    Py::Object setViewportRegion(const Py::Tuple&);
    Py::Object getViewportRegion();

    Py::Object setBackgroundColor(const Py::Tuple&);
    Py::Object getBackgroundColor();

    Py::Object setNumPasses(const Py::Tuple&);
    Py::Object getNumPasses();

    Py::Object setInternalTextureFormat(const Py::Tuple&);
    Py::Object getInternalTextureFormat();

    Py::Object render(const Py::Tuple&);

    Py::Object writeToImage(const Py::Tuple&);
    Py::Object getWriteImageFiletypeInfo();

private:
    SoQtOffscreenRenderer renderer;
};

}  // namespace Gui

#endif  // GUI_SOQTOFFSCREENRENDERERPY_H
