#ifndef GUI_TASKVIEW_TaskPostExtraction_H
#define GUI_TASKVIEW_TaskPostExtraction_H

#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/ViewProviderDocumentObject.h>

#include <QAbstractTableModel>

#include "TaskPostBoxes.h"

#include <vtkSmartPointer.h>
#include <vtkTableAlgorithm.h>

class Ui_TaskPostExtraction;


namespace FemGui
{

// ***************************************************************************
// box to handle data extractions: It is implemented in python, the c++
// code is used to access it and manage it for the c++ task panels
class TaskPostExtraction: public TaskPostWidget
{
    Q_OBJECT

public:
    explicit TaskPostExtraction(ViewProviderFemPostObject* view, QWidget* parent = nullptr);
    ~TaskPostExtraction();

protected:
    bool isGuiTaskOnly() override;
    void apply() override;
    void onPostDataChanged(Fem::FemPostObject* obj) override;
    bool initiallyCollapsed() override;

private:
    Py::Object m_panel;
};


}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskPostExtraction_H
