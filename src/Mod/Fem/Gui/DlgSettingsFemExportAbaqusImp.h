#ifndef FEMGUI_DLGSETTINGSFEMEXPORTABAQUSIMP_H
#define FEMGUI_DLGSETTINGSFEMEXPORTABAQUSIMP_H

#include <memory>

#include <Gui/PropertyPage.h>


namespace FemGui
{

class Ui_DlgSettingsFemExportAbaqus;
class DlgSettingsFemExportAbaqusImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemExportAbaqusImp(QWidget* parent = nullptr);
    ~DlgSettingsFemExportAbaqusImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsFemExportAbaqus> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMEXPORTABAQUSIMP_H
