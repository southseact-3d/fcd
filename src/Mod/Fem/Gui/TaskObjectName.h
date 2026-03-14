#ifndef GUI_TASKVIEW_TaskObjectName_H
#define GUI_TASKVIEW_TaskObjectName_H

#include <Gui/TaskView/TaskView.h>


class Ui_TaskObjectName;

namespace App
{
class Property;
class DocumentObject;
}  // namespace App

namespace Gui
{
class ViewProvider;
}

namespace FemGui
{


class TaskObjectName: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskObjectName(App::DocumentObject* pcObject, QWidget* parent = nullptr);
    ~TaskObjectName() override;


    std::string name;

private Q_SLOTS:
    void TextChanged(const QString&);

protected:
    App::DocumentObject* pcObject;

private:
private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskObjectName> ui;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskObjectName_H
