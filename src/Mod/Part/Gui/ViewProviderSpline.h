#ifndef PARTGUI_VIEWPROVIDERPARTSPLINE_H
#define PARTGUI_VIEWPROVIDERPARTSPLINE_H

#include <Mod/Part/Gui/ViewProviderExt.h>
#include <Gui/ViewProviderExtensionPython.h>

#include <Mod/Part/PartGlobal.h>

namespace PartGui
{

class PartGuiExport ViewProviderSplineExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderSplineExtension);

public:
    /// Constructor
    ViewProviderSplineExtension();
    ~ViewProviderSplineExtension() override = default;

    App::PropertyBool ControlPoints;

    void extensionUpdateData(const App::Property*) override;
    void extensionSetupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    void extensionOnChanged(const App::Property* p) override;
    void toggleControlPoints(bool);
    void showControlPoints(bool, const App::Property* prop);
    void showControlPointsOfEdge(const TopoDS_Edge&);
    void showControlPointsOfFace(const TopoDS_Face&);

    SoSwitch* pcControlPoints {nullptr};
};

class PartGuiExport ViewProviderSpline: public ViewProviderPartExt
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderSpline);

public:
    /// constructor
    ViewProviderSpline();
    /// destructor
    ~ViewProviderSpline() override;

    QIcon getIcon() const override;

private:
    ViewProviderSplineExtension extension;
};

using ViewProviderSplineExtensionPython
    = Gui::ViewProviderExtensionPythonT<PartGui::ViewProviderSplineExtension>;

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPARTSPLINE_H
