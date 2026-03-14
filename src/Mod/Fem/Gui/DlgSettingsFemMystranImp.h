#ifndef FEMGUI_DLGSETTINGSFEMMYSTRANIMP_H
#define FEMGUI_DLGSETTINGSFEMMYSTRANIMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{

class Ui_DlgSettingsFemMystranImp;
class DlgSettingsFemMystranImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemMystranImp(QWidget* parent = nullptr);
    ~DlgSettingsFemMystranImp() override;

protected Q_SLOTS:
    void onfileNameSelected(const QString& fileName);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemMystranImp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMMYSTRANIMP_H
