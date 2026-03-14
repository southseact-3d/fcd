#ifndef GUI_VIEW3DSETTINGS_H
#define GUI_VIEW3DSETTINGS_H

#include <Base/Parameter.h>
#include <QApplication>

namespace Gui
{
class View3DInventorViewer;

class GuiExport View3DSettings: public ParameterGrp::ObserverType
{
public:
    static constexpr auto defaultHeadLightDirection = "(0.6841049,-0.12062616,-0.7193398)";
    static constexpr auto defaultFillLightDirection = "(-0.6403416,0.7631294,0.087155744)";
    static constexpr auto defaultBackLightDirection = "(-0.7544065,-0.63302225,-0.17364818)";

    View3DSettings(ParameterGrp::handle hGrp, View3DInventorViewer*);
    View3DSettings(ParameterGrp::handle hGrp, const std::vector<View3DInventorViewer*>&);
    ~View3DSettings() override;

    /// Observer message from the ParameterGrp
    void OnChange(ParameterGrp::SubjectType& rCaller, ParameterGrp::MessageType Reason) override;
    void applySettings();
    int stopAnimatingIfDeactivated() const;

    bool ignoreNavigationStyle = false;
    bool ignoreVBO = false;
    bool ignoreDimensions = false;
    bool ignoreRenderCache = false;
    bool ignoreTransparent = false;

private:
    ParameterGrp::handle hGrp;
    ParameterGrp::handle hLightSourcesGrp;

    std::vector<View3DInventorViewer*> _viewers;
};

class NaviCubeSettings
{
    Q_DECLARE_TR_FUNCTIONS(NaviCubeSettings)
public:
    NaviCubeSettings(ParameterGrp::handle hGrp, View3DInventorViewer*);
    ~NaviCubeSettings();

    void applySettings();

private:
    void parameterChanged(ParameterGrp::MessageType pName);
    ParameterGrp::handle hGrp;
    View3DInventorViewer* _viewer;
    fastsignals::connection connectParameterChanged;
};

}  // namespace Gui

#endif  // GUI_VIEW3DSETTINGS_H
