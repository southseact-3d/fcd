#ifndef MATGUI_DLGSETTINGSMATERIAL_H
#define MATGUI_DLGSETTINGSMATERIAL_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace MatGui
{
class Ui_DlgSettingsMaterial;

class DlgSettingsMaterial: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsMaterial(QWidget* parent = nullptr);
    ~DlgSettingsMaterial() override = default;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsMaterial> ui;
};

}  // namespace MatGui

#endif  // MATGUI_DLGSETTINGSMATERIAL_H
