#ifndef FEMGUI_DLGSETTINGSFEMCCXIMP_H
#define FEMGUI_DLGSETTINGSFEMCCXIMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{
class Ui_DlgSettingsFemCcxImp;
class DlgSettingsFemCcxImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemCcxImp(QWidget* parent = nullptr);
    ~DlgSettingsFemCcxImp() override;

protected Q_SLOTS:
    void onfileNameSelected(const QString& fileName);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemCcxImp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMCCXIMP_H
