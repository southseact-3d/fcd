#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPCOLORS_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPCOLORS_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui {
class Ui_DlgPrefsTechDrawColorsImp;

class DlgPrefsTechDrawColorsImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawColorsImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawColorsImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawColorsImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPCOLORS_H
