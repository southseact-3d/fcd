#ifndef PARTGUI_DIALOG_DLGSETTINGSOBJECTCOLOR_H
#define PARTGUI_DIALOG_DLGSETTINGSOBJECTCOLOR_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace PartGui
{
class Ui_DlgSettingsObjectColor;

/**
 * The DlgSettingsObjectColor class implements a preference page to change color settings
 * for data objects.
 * @author Werner Mayer
 */
class DlgSettingsObjectColor: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsObjectColor(QWidget* parent = nullptr);
    ~DlgSettingsObjectColor() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsObjectColor> ui;
};

}  // namespace PartGui

#endif  // PARTGUI_DIALOG_DLGSETTINGSOBJECTCOLOR_H
