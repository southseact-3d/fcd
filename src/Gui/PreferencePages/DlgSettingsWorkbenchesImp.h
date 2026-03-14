#ifndef GUI_DIALOG_DLGSETTINGSWORKBENCHES_IMP_H
#define GUI_DIALOG_DLGSETTINGSWORKBENCHES_IMP_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsWorkbenches;

/**
 * The DlgSettingsWorkbenchesImp class implements a pseudo-preference page explain why
 * the remaining preference pages aren't loaded yet, and to help the user do so on demand.
 * \author Jürgen Riegel
 */
class DlgSettingsWorkbenchesImp: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsWorkbenchesImp(QWidget* parent = nullptr);
    ~DlgSettingsWorkbenchesImp() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

    static QStringList getEnabledWorkbenches();
    static QStringList getDisabledWorkbenches();

protected Q_SLOTS:
    void wbToggled(const QString& wbName, bool enabled);
    void wbItemMoved();
    void onStartWbChanged(int index);
    void onWbByTabToggled(bool val);

protected:
    void buildWorkbenchList();
    void changeEvent(QEvent* e) override;

private:
    void addWorkbench(const QString& it, bool enabled);
    void sortEnabledWorkbenches();

    void setStartWorkbenchComboItems();

    void saveWorkbenchSelector();
    void loadWorkbenchSelector();
    void translateWorkbenchSelector();


    std::vector<std::string> _backgroundAutoloadedModules;
    std::string _startupModule;

    std::unique_ptr<Ui_DlgSettingsWorkbenches> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSWORKBENCHES_IMP_H
