#ifndef FEMGUI_DLGSETTINGSFEMGMSHIMP_H
#define FEMGUI_DLGSETTINGSFEMGMSHIMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{
class Ui_DlgSettingsFemGmshImp;
class DlgSettingsFemGmshImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemGmshImp(QWidget* parent = nullptr);
    ~DlgSettingsFemGmshImp() override;

protected Q_SLOTS:
    void onfileNameSelected(const QString& fileName);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;
    void populateLogVerbosity();

private:
    std::unique_ptr<Ui_DlgSettingsFemGmshImp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMGMSHIMP_H
