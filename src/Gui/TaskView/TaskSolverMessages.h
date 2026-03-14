#ifndef GUI_TASKVIEW_TaskSolverMessages_H
#define GUI_TASKVIEW_TaskSolverMessages_H

#include <fastsignals/signal.h>

#include <Gui/TaskView/TaskView.h>

class Ui_TaskSolverMessages;
using Connection = fastsignals::connection;

namespace App
{
class Property;
}

namespace Gui
{

class GuiExport TaskSolverMessages: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskSolverMessages(const QPixmap& icon, const QString& title);
    ~TaskSolverMessages() override;
    FC_DISABLE_COPY_MOVE(TaskSolverMessages)

    void slotSetUp(const QString& state, const QString& msg, const QString& link, const QString& linkText);

private:
    void setupConnections();
    virtual void createSettingsButtonActions();
    virtual void onLabelStatusLinkClicked(const QString&) = 0;
    void onManualUpdateClicked(bool checked);

    virtual void updateToolTip(const QString&) = 0;

protected:
    void setLinkTooltip(const QString& tooltip);
    QToolButton* getSettingsButton();

    Connection connectionSetUp;

private:
    std::unique_ptr<Ui_TaskSolverMessages> ui;
};

}  // namespace Gui

#endif  // GUI_TASKVIEW_TaskSolverMessages_H
