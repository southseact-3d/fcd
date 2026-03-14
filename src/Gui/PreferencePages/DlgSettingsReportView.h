#ifndef GUI_DIALOG_DLG_SETTINGS_REPORT_VIEW
#define GUI_DIALOG_DLG_SETTINGS_REPORT_VIEW

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsReportView;

/** The DlgSettingsReportView class implements the available settings for the
 * Report View to change.
 * \author Werner Mayer
 */
class DlgSettingsReportView: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsReportView(QWidget* parent = nullptr);
    ~DlgSettingsReportView() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsReportView> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLG_SETTINGS_REPORT_VIEW
