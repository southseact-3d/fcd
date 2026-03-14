#ifndef PARTGUI_BOXSELECTION_H
#define PARTGUI_BOXSELECTION_H

#include <TopAbs_ShapeEnum.hxx>

class SoEventCallback;
class TopoDS_Shape;

namespace Base
{
class Polygon2d;
}

namespace Gui
{
class View3DInventorViewer;
class ViewVolumeProjection;
}  // namespace Gui

namespace PartGui
{

class BoxSelection
{
public:
    BoxSelection();
    ~BoxSelection();

    void setAutoDelete(bool);
    bool isAutoDelete() const;
    void start(TopAbs_ShapeEnum shape);

private:
    class FaceSelectionGate;
    void addShapeToSelection(
        const char* doc,
        const char* obj,
        const Gui::ViewVolumeProjection& proj,
        const Base::Polygon2d& polygon,
        const TopoDS_Shape& shape,
        TopAbs_ShapeEnum subtype
    );
    const char* nameFromShapeType(TopAbs_ShapeEnum) const;
    static void selectionCallback(void* ud, SoEventCallback* cb);

private:
    bool autodelete {false};
    TopAbs_ShapeEnum shapeEnum {TopAbs_SHAPE};
};

}  // namespace PartGui

#endif  // PARTGUI_BOXSELECTION_H
