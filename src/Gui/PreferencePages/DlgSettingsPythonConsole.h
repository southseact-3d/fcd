#ifndef GUI_DIALOG_DLGSETTINGSPYTHONCONSOLE_H
#define GUI_DIALOG_DLGSETTINGSPYTHONCONSOLE_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsPythonConsole;

/**
 * The DlgSettingsPythonConsole class implements a preference page to change settings
 * for the Python console.
 * @author Werner Mayer
 */
class DlgSettingsPythonConsole: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsPythonConsole(QWidget* parent = nullptr);
    ~DlgSettingsPythonConsole() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsPythonConsole> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSPYTHONCONSOLE_H
