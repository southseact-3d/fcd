#ifndef GUI_TASKVIEW_TaskPadParameters_H
#define GUI_TASKVIEW_TaskPadParameters_H

#include "TaskExtrudeParameters.h"
#include "ViewProviderPad.h"

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


class TaskPadParameters: public TaskExtrudeParameters
{
    Q_OBJECT

public:
    explicit TaskPadParameters(ViewProviderPad* PadView, QWidget* parent = nullptr, bool newObj = false);
    ~TaskPadParameters() override;

    void apply() override;

private:
    void onModeChanged(int index, Side side) override;
    void translateModeList(QComboBox* box, int index) override;
    void updateUI(Side side) override;
};

/// simulation dialog for the TaskView
class TaskDlgPadParameters: public TaskDlgExtrudeParameters
{
    Q_OBJECT

public:
    explicit TaskDlgPadParameters(ViewProviderPad* PadView, bool newObj = false);

protected:
    TaskExtrudeParameters* getTaskParameters() override
    {
        return parameters;
    }

private:
    TaskPadParameters* parameters;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
