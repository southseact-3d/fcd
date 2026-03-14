#ifndef GUI_TASKVIEW_TaskPocketParameters_H
#define GUI_TASKVIEW_TaskPocketParameters_H

#include "TaskExtrudeParameters.h"
#include "ViewProviderPocket.h"

class QComboBox;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace PartDesignGui
{


class TaskPocketParameters: public TaskExtrudeParameters
{
    Q_OBJECT

public:
    explicit TaskPocketParameters(
        ViewProviderPocket* PocketView,
        QWidget* parent = nullptr,
        bool newObj = false
    );
    ~TaskPocketParameters() override;

    void apply() override;

private:
    void onModeChanged(int index, Side side) override;
    void translateModeList(QComboBox* box, int index) override;
    void updateUI(Side side) override;
};

/// simulation dialog for the TaskView
class TaskDlgPocketParameters: public TaskDlgExtrudeParameters
{
    Q_OBJECT

public:
    explicit TaskDlgPocketParameters(ViewProviderPocket* PocketView);

protected:
    TaskExtrudeParameters* getTaskParameters() override
    {
        return parameters;
    }

private:
    TaskPocketParameters* parameters;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
