#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPADVANCED_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPADVANCED_H

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include <memory>


namespace TechDrawGui {
class Ui_DlgPrefsTechDrawAdvancedImp;

class DlgPrefsTechDrawAdvancedImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawAdvancedImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawAdvancedImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *event) override;

    void loadBalloonOverride();
    void saveBalloonOverride();

    void clearBalloonOptions();

    static bool flagsContainValue(uint flags, uint value);

    void makeBalloonBoxConnections();
    void slotBalloonBoxChecked();
    void enableBalloonOptions(bool newState);


private:
    std::unique_ptr<Ui_DlgPrefsTechDrawAdvancedImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPADVANCED_H
