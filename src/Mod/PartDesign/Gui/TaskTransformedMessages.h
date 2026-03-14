#ifndef GUI_TASKVIEW_TaskTransformedMessages_H
#define GUI_TASKVIEW_TaskTransformedMessages_H

#include <Gui/TaskView/TaskView.h>

class Ui_TaskTransformedMessages;
using Connection = fastsignals::connection;

namespace App
{
class Property;
}

namespace PartDesignGui
{

class ViewProviderTransformed;

class TaskTransformedMessages: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskTransformedMessages(ViewProviderTransformed* transformedView);
    ~TaskTransformedMessages() override;

    void slotDiagnosis(QString msg);

private Q_SLOTS:

protected:
    ViewProviderTransformed* transformedView;
    Connection connectionDiagnosis;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskTransformedMessages> ui;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskTransformedMessages_H
