#ifndef FEMGUI_DLGSETTINGSFEMZ88IMP_H
#define FEMGUI_DLGSETTINGSFEMZ88IMP_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace FemGui
{
class Ui_DlgSettingsFemZ88Imp;

class DlgSettingsFemZ88Imp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemZ88Imp(QWidget* parent = nullptr);
    ~DlgSettingsFemZ88Imp() override;

protected Q_SLOTS:
    void onfileNameSelected(const QString& fileName);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemZ88Imp> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMZ88IMP_H
