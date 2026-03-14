#ifndef GUI_DIALOG_DLGSETTINGSLIGHTSOURCES_H
#define GUI_DIALOG_DLGSETTINGSLIGHTSOURCES_H

#include <Gui/PropertyPage.h>
#include <memory>
#include <QPointer>
#include <App/Application.h>
#include <Base/Parameter.h>
#include <Base/Vector3D.h>

class SoDragger;
class SbRotation;
class SoDirectionalLightDragger;
class SoOrthographicCamera;

namespace Gui
{
class View3DInventorViewer;
namespace Dialog
{
class Ui_DlgSettingsLightSources;

/**
 * The DlgSettingsLightSources class implements a preference page to change settings
 * for the light sources of a 3D view.
 * @author Werner Mayer
 */
class DlgSettingsLightSources: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsLightSources(QWidget* parent = nullptr);
    ~DlgSettingsLightSources() override = default;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

public Q_SLOTS:
    void zoomIn() const;
    void zoomOut() const;

protected:
    void changeEvent(QEvent* event) override;

private:
    void configureViewer();

    Base::Vector3d azimuthElevationToDirection(double azimuth, double elevation);
    std::pair<double, double> directionToAzimuthElevation(Base::Vector3d direction);

private:
    std::unique_ptr<Ui_DlgSettingsLightSources> ui;
    QPointer<View3DInventorViewer> view;
    SoOrthographicCamera* camera = nullptr;

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/View/LightSources"
    );
    ParameterGrp::handle hGrpView = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/View"
    );

    float zoomStep = 3.0f;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSLIGHTSOURCES_H
