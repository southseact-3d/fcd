#ifndef MATGUI_DLGSETTINGSDEFAULTMATERIAL_H
#define MATGUI_DLGSETTINGSDEFAULTMATERIAL_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace MatGui
{
class Ui_DlgSettingsDefaultMaterial;

class DlgSettingsDefaultMaterial: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsDefaultMaterial(QWidget* parent = nullptr);
    ~DlgSettingsDefaultMaterial() override = default;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsDefaultMaterial> ui;

    void setupFilters();
};

}  // namespace MatGui

#endif  // MATGUI_DLGSETTINGSDEFAULTMATERIAL_H
