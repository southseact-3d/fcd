#ifndef MESHPARTGUI_CURVEONMESH_H
#define MESHPARTGUI_CURVEONMESH_H

#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <QObject>
#include <memory>

#include <Gui/ViewProviderDocumentObject.h>


class SbVec3f;
class SoCoordinate3;
class SoDrawStyle;
class TopoDS_Edge;
class TopoDS_Wire;

namespace Gui
{
class View3DInventor;
class ViewProvider;
}  // namespace Gui

namespace MeshPartGui
{

class ViewProviderCurveOnMesh: public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(MeshPartGui::ViewProviderCurveOnMesh);

public:
    ViewProviderCurveOnMesh();
    ~ViewProviderCurveOnMesh() override;
    void addVertex(const SbVec3f&);
    void clearVertex();
    void setPoints(const std::vector<SbVec3f>&);
    void clearPoints();
    void setDisplayMode(const char* ModeName) override;

private:
    SoCoordinate3* pcCoords;
    SoCoordinate3* pcNodes;
    SoDrawStyle* pcPointStyle;
    SoDrawStyle* pcLinesStyle;
};

class CurveOnMeshHandler: public QObject
{
    Q_OBJECT

public:
    explicit CurveOnMeshHandler(QObject* parent = nullptr);
    ~CurveOnMeshHandler() override;
    void enableApproximation(bool);
    void setParameters(int maxDegree, GeomAbs_Shape cont, double tol3d, double angle);
    void enableCallback(Gui::View3DInventor* viewer);
    void disableCallback();
    void recomputeDocument();

private:
    Handle(Geom_BSplineCurve) approximateSpline(const std::vector<SbVec3f>& points);
    void approximateEdge(const TopoDS_Edge&, double tolerance);
    void displaySpline(const Handle(Geom_BSplineCurve) &);
    bool makePolyline(const std::vector<SbVec3f>& points, TopoDS_Wire& wire);
    void displayPolyline(const TopoDS_Wire& wire);
    std::vector<SbVec3f> getPoints() const;
    std::vector<SbVec3f> getVertexes() const;
    void closeWire();
    bool tryCloseWire(const SbVec3f&) const;

private Q_SLOTS:
    void onContextMenu();
    void onCreate();
    void onClear();
    void onCancel();
    void onCloseWire();

private:
    class Private;
    std::unique_ptr<Private> d_ptr;
};

}  // namespace MeshPartGui

#endif  // MESHPARTGUI_CURVEONMESH_H
