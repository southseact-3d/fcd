#ifndef GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H
#define GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H

#include <Gui/PropertyPage.h>
#include <memory>

class QDoubleSpinBox;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettings3DView;

/**
 * The DlgSettings3DViewImp class implements a preference page to change settings
 * for the Inventor viewer.
 * \author Jürgen Riegel
 */
class DlgSettings3DViewImp: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettings3DViewImp(QWidget* parent = nullptr);
    ~DlgSettings3DViewImp() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

private Q_SLOTS:
    void onAliasingChanged(int);

protected:
    void changeEvent(QEvent* e) override;

private:
    void addAntiAliasing();
    void saveAntiAliasing();
    void loadAntiAliasing();
    void saveRenderCache();
    void loadRenderCache();
    void saveMarkerSize();
    void loadMarkerSize();

private:
    std::unique_ptr<Ui_DlgSettings3DView> ui;

    Q_DISABLE_COPY_MOVE(DlgSettings3DViewImp)
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H
