#ifndef PARTGUI_TASKTHICKNESS_H
#define PARTGUI_TASKTHICKNESS_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>

namespace Gui
{
class LinearGizmo;
class GizmoContainer;
}  // namespace Gui

namespace Part
{
class Thickness;
}
namespace PartGui
{

class ThicknessWidget: public QWidget
{
    Q_OBJECT

public:
    explicit ThicknessWidget(Part::Thickness*, QWidget* parent = nullptr);
    ~ThicknessWidget() override;

    bool accept();
    bool reject();
    Part::Thickness* getObject() const;

private:
    void setupConnections();
    void onSpinOffsetValueChanged(double);
    void onModeTypeActivated(int);
    void onJoinTypeActivated(int);
    void onIntersectionToggled(bool);
    void onSelfIntersectionToggled(bool);
    void onFacesButtonToggled(bool);
    void onUpdateViewToggled(bool);

private:
    void changeEvent(QEvent* e) override;

    std::unique_ptr<Gui::GizmoContainer> gizmoContainer;
    Gui::LinearGizmo* linearGizmo = nullptr;
    void setupGizmos();
    void setGizmoPositions();

private:
    class Private;
    Private* d;
};

class TaskThickness: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskThickness(Part::Thickness*);

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;
    Part::Thickness* getObject() const;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    ThicknessWidget* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKTHICKNESS_H
