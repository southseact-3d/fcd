#ifndef DRAWINGGUI_DLGPREFSTECHDRAWIMPANNOTATION_H
#define DRAWINGGUI_DLGPREFSTECHDRAWIMPANNOTATION_H

#include <memory>
#include <Gui/PrefWidgets.h>
#include <Gui/PropertyPage.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

namespace TechDraw {
class LineGenerator;
enum class ArrowType : int;
}

namespace TechDrawGui {
class Ui_DlgPrefsTechDrawAnnotationImp;

class DlgPrefsTechDrawAnnotationImp : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsTechDrawAnnotationImp( QWidget* parent = nullptr );
    ~DlgPrefsTechDrawAnnotationImp() override;

public Q_SLOTS:
    void onLineGroupChanged(int);
    void onLineStandardChanged(int);

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent *e) override;

    TechDraw::ArrowType prefBalloonArrow() const;
    int prefBalloonShape() const;
    int prefMattingStyle() const;
    void loadLineStyleBoxes();

private:
    std::unique_ptr<Ui_DlgPrefsTechDrawAnnotationImp> ui;
    TechDraw::LineGenerator* m_lineGenerator;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGPREFSTECHDRAWIMPANNOTATION_H
