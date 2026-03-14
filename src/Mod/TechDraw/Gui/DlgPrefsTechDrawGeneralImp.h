#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPGENERAL_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPGENERAL_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui {
class Ui_DlgPrefsTechDrawGeneralImp;

class DlgPrefsTechDrawGeneralImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawGeneralImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawGeneralImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawGeneralImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPGENERAL_H
