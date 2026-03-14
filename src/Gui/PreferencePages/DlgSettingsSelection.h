#ifndef GUI_DIALOG_DLGSETTINGSSELECTION_H
#define GUI_DIALOG_DLGSETTINGSSELECTION_H

#include <Gui/PropertyPage.h>
#include <memory>

class QDoubleSpinBox;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsSelection;

/**
 * The Ui_DlgSettingsSelection class implements a preference page to change settings
 * for the selection.
 * \author Werner Mayer
 */
class DlgSettingsSelection: public PreferencePage
{
    Q_OBJECT

public:
    DlgSettingsSelection(QWidget* parent = nullptr);
    ~DlgSettingsSelection() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsSelection> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSSELECTION_H
