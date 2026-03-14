#ifndef PATHGUI_DIALOG_DLGSETTINGSPATHCOLOR_H
#define PATHGUI_DIALOG_DLGSETTINGSPATHCOLOR_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace PathGui
{
class Ui_DlgSettingsPathColor;

class DlgSettingsPathColor: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsPathColor(QWidget* parent = nullptr);
    ~DlgSettingsPathColor() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsPathColor> ui;
};

}  // namespace PathGui

#endif  // PATHGUI_DIALOG_DLGSETTINGSPATHCOLOR_H
