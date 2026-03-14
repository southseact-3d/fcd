#ifndef GUI_DIALOG_DLGSETTINGSNOTIFICATIONAREA_H
#define GUI_DIALOG_DLGSETTINGSNOTIFICATIONAREA_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsNotificationArea;

/**
 * The DlgSettingsNotificationArea class implements a preference page to change settings
 * for the Notification Area.
 */
class DlgSettingsNotificationArea: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsNotificationArea(QWidget* parent = nullptr);
    ~DlgSettingsNotificationArea() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsNotificationArea> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSNOTIFICATIONAREA_H
