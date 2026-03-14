#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPSCALE_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPSCALE_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui {
class Ui_DlgPrefsTechDrawScaleImp;

class DlgPrefsTechDrawScaleImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawScaleImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawScaleImp() override;

protected Q_SLOTS:
    void onScaleTypeChanged(int index);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawScaleImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPSCALE_H
