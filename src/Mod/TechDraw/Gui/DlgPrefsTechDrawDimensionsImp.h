#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPDIMENSIONS_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPDIMENSIONS_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw {
enum class ArrowType : int;
}

namespace TechDrawGui {
class Ui_DlgPrefsTechDrawDimensionsImp;

class DlgPrefsTechDrawDimensionsImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawDimensionsImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawDimensionsImp() override;

    void resetSettingsToDefaults() override;
protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;
    void dimensioningModeChanged(int index);

    TechDraw::ArrowType prefArrowStyle() const;

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawDimensionsImp> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPDIMENSIONS_H
