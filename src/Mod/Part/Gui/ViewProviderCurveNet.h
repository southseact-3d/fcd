#ifndef PARTGUI_VIEWPROVIDERCURVENET_H
#define PARTGUI_VIEWPROVIDERCURVENET_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace Gui
{
class View3DInventorViewer;
class SoFCSelection;
}  // namespace Gui

namespace PartGui
{


class PartGuiExport ViewProviderCurveNet: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPart);

public:
    /// constructor
    ViewProviderCurveNet();
    /// destructor
    ~ViewProviderCurveNet() override;

    void attach(App::DocumentObject*) override;
    void setDisplayMode(const char* ModeName) override;
    /// returns a list of all possible modes
    std::vector<std::string> getDisplayModes() const override;

    /// Update the Part representation
    void updateData(const App::Property*) override;

    virtual bool handleEvent(const SoEvent* const ev, Gui::View3DInventorViewer& Viewer);

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;

    struct Node
    {
        Gui::SoFCSelection* pcHighlight;
        SoTransform* pcTransform;
        Node()
        {
            pcHighlight = nullptr;
            pcTransform = nullptr;
        }
    };

    std::list<Node> NodeList;

    bool bInEdit {false};
    bool bMovePointMode {false};
    Node PointToMove;
    /// root of the edge and vertex points
    SoSeparator *EdgeRoot {nullptr}, *VertexRoot {nullptr};

    Standard_Boolean computeEdges(SoSeparator* root, const TopoDS_Shape& myShape);
    Standard_Boolean computeVertices(SoSeparator* root, const TopoDS_Shape& myShape);
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERCURVENET_H
