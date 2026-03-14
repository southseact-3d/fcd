#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPHLR_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPHLR_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui {
class Ui_DlgPrefsTechDrawHLRImp;

class DlgPrefsTechDrawHLRImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawHLRImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawHLRImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawHLRImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPHLR_H
