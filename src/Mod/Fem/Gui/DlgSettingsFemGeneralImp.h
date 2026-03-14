#ifndef FEMGUI_DLGSETTINGSFEMGENERALIMP_H
#define FEMGUI_DLGSETTINGSFEMGENERALIMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{
class Ui_DlgSettingsFemGeneralImp;
class DlgSettingsFemGeneralImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemGeneralImp(QWidget* parent = nullptr);
    ~DlgSettingsFemGeneralImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemGeneralImp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMGENERALIMP_H
