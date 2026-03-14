#ifndef SKETCHERGUI_TASKSKETCHERVALIDATION_H
#define SKETCHERGUI_TASKSKETCHERVALIDATION_H

#include <memory>
#include <vector>

#include <App/DocumentObserver.h>
#include <Base/Vector3D.h>
#include <Gui/TaskView/TaskDialog.h>


class SoGroup;
namespace Sketcher
{
class SketchObject;
}

namespace SketcherGui
{

class Ui_TaskSketcherValidation;
class SketcherValidation: public QWidget
{
    Q_OBJECT

public:
    explicit SketcherValidation(Sketcher::SketchObject* Obj, QWidget* parent = nullptr);
    ~SketcherValidation() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();
    void onFindButtonClicked();
    void onFixButtonClicked();
    void onHighlightButtonClicked();
    void onFindConstraintClicked();
    void onFixConstraintClicked();
    void onFindReversedClicked();
    void onSwapReversedClicked();
    void onOrientLockEnableClicked();
    void onOrientLockDisableClicked();
    void onDelConstrExtrClicked();
    void onFindDegeneratedClicked();
    void onFixDegeneratedClicked();

private:
    void showPoints(const std::vector<Base::Vector3d>&);
    void hidePoints();

private:
    std::unique_ptr<Ui_TaskSketcherValidation> ui;
    App::WeakPtrT<Sketcher::SketchObject> sketch;
    SoGroup* coincidenceRoot;
};

class TaskSketcherValidation: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskSketcherValidation(Sketcher::SketchObject* Obj);
    ~TaskSketcherValidation() override;
    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close;
    }
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_TASKSKETCHERVALIDATION_H
