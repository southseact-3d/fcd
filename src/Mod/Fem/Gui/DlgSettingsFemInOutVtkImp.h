#ifndef FEMGUI_DLGSETTINGSFEMINOUTVTKIMP_H
#define FEMGUI_DLGSETTINGSFEMINOUTVTKIMP_H

#include <memory>

#include <Gui/PropertyPage.h>


namespace FemGui
{

class Ui_DlgSettingsFemInOutVtk;
class DlgSettingsFemInOutVtkImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsFemInOutVtkImp(QWidget* parent = nullptr);
    ~DlgSettingsFemInOutVtkImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    void populateExportLevel() const;
    std::unique_ptr<Ui_DlgSettingsFemInOutVtk> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_DLGSETTINGSFEMINOUTVTKIMP_H
