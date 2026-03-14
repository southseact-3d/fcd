#ifndef MESHGUI_DLGSETTINGSMESHVIEW_H
#define MESHGUI_DLGSETTINGSMESHVIEW_H

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif
#include <memory>

#include <Gui/PropertyPage.h>


namespace MeshGui
{
class Ui_DlgSettingsMeshView;
/**
 * The DlgSettingsMeshView class implements a preference page to change settings
 * for display of meshes.
 * @author Werner Mayer
 */
class DlgSettingsMeshView: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsMeshView(QWidget* parent = nullptr);
    ~DlgSettingsMeshView() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsMeshView> ui;

    Q_DISABLE_COPY_MOVE(DlgSettingsMeshView)
};

}  // namespace MeshGui

#endif  // MESHGUI_DLGSETTINGSMESHVIEW_H
