#ifndef FEMGUI_DLGSETTINGSFEMELMERIMP_H
#define FEMGUI_DLGSETTINGSFEMELMERIMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{
class Ui_DlgSettingsFemElmerImp;
class DlgSettingsFemElmerImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemElmerImp(QWidget* parent = nullptr);
    ~DlgSettingsFemElmerImp() override;

protected Q_SLOTS:
    void onfileNameSelected(const QString& fileName);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemElmerImp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMELMERIMP_H
