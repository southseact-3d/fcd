#ifndef GUI_DIALOG_DLGSETTINGSVIEWCOLOR_H
#define GUI_DIALOG_DLGSETTINGSVIEWCOLOR_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsViewColor;

/**
 * The DlgSettingsViewColor class implements a preference page to change color settings
 * for the Inventor viewer like background and selection.
 * @author Werner Mayer
 */
class DlgSettingsViewColor: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsViewColor(QWidget* parent = nullptr);
    ~DlgSettingsViewColor() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

protected Q_SLOTS:
    void onSwitchGradientColorsPressed();
    void onRadioButtonSimpleToggled(bool val);
    void onRadioButtonGradientToggled(bool val);
    void onRadioButtonRadialGradientToggled(bool val);
    void onCheckMidColorToggled(bool val);

private:
    void setGradientColorVisibility(bool val);

private:
    std::unique_ptr<Ui_DlgSettingsViewColor> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSVIEWCOLOR_H
