#ifndef GUI_DIALOG_DLGSETTINGSMACROIMP_H
#define GUI_DIALOG_DLGSETTINGSMACROIMP_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsMacro;

/**
 * The DlgSettingsMacroImp class implements a preference page to change settings
 * for macro stuff.
 * \author Jürgen Riegel
 */
class DlgSettingsMacroImp: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsMacroImp(QWidget* parent = nullptr);
    ~DlgSettingsMacroImp() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsMacro> ui;
    void setRecentMacroSize();
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSMACROIMP_H
