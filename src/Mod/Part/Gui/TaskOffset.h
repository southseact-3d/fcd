#ifndef PARTGUI_TASKOFFSET_H
#define PARTGUI_TASKOFFSET_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>

namespace Part
{
class Offset;
}
namespace PartGui
{

class OffsetWidget: public QWidget
{
    Q_OBJECT

public:
    explicit OffsetWidget(Part::Offset*, QWidget* parent = nullptr);
    ~OffsetWidget() override;

    bool accept();
    bool reject();
    Part::Offset* getObject() const;

private:
    void setupConnections();
    void onSpinOffsetValueChanged(double);
    void onModeTypeActivated(int);
    void onJoinTypeActivated(int);
    void onIntersectionToggled(bool);
    void onSelfIntersectionToggled(bool);
    void onFillOffsetToggled(bool);
    void onUpdateViewToggled(bool);

private:
    void changeEvent(QEvent* e) override;

private:
    class Private;
    Private* d;
};

class TaskOffset: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskOffset(Part::Offset*);
    ~TaskOffset() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;
    Part::Offset* getObject() const;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    OffsetWidget* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKOFFSET_H
