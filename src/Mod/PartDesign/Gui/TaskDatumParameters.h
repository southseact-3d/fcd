#ifndef GUI_TASKVIEW_TaskDatumParameters_H
#define GUI_TASKVIEW_TaskDatumParameters_H

#include <Gui/TaskView/TaskView.h>
#include <Mod/Part/Gui/TaskAttacher.h>

#include "ViewProviderDatum.h"

class Ui_TaskDatumParameters;
class QLineEdit;

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


class TaskDatumParameters: public PartGui::TaskAttacher
{
    Q_OBJECT

public:
    explicit TaskDatumParameters(ViewProviderDatum* DatumView, QWidget* parent = nullptr);
    ~TaskDatumParameters() override;
};

/// simulation dialog for the TaskView
class TaskDlgDatumParameters: public PartGui::TaskDlgAttacher
{
    Q_OBJECT

public:
    explicit TaskDlgDatumParameters(ViewProviderDatum* DatumView);
    ~TaskDlgDatumParameters() override;

    bool accept() override;
    bool reject() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
