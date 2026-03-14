#ifndef PARTGUI_TASKLOFT_H
#define PARTGUI_TASKLOFT_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>

class QTreeWidgetItem;

namespace PartGui
{

class LoftWidget: public QWidget
{
    Q_OBJECT

public:
    explicit LoftWidget(QWidget* parent = nullptr);
    ~LoftWidget() override;

    bool accept();
    bool reject();

private Q_SLOTS:
    void onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

private:
    void changeEvent(QEvent* e) override;
    void findShapes();

private:
    class Private;
    Private* d;
};

class TaskLoft: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskLoft();
    ~TaskLoft() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    LoftWidget* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKLOFT_H
