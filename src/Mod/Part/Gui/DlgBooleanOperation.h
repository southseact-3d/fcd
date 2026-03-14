#ifndef PARTGUI_DLGBOOLEANOPERATION_H
#define PARTGUI_DLGBOOLEANOPERATION_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


class QTreeWidgetItem;

namespace App
{
class DocumentObject;
class Property;
}  // namespace App
namespace PartGui
{

using Connection = fastsignals::connection;
class Ui_DlgBooleanOperation;
class DlgBooleanOperation: public QWidget
{
    Q_OBJECT

public:
    explicit DlgBooleanOperation(QWidget* parent = nullptr);
    ~DlgBooleanOperation() override;
    void accept();

private:
    void findShapes();
    bool indexOfCurrentItem(QTreeWidgetItem*, int&, int&) const;
    void slotCreatedObject(const App::DocumentObject&);
    void slotChangedObject(const App::DocumentObject&, const App::Property&);
    bool hasSolids(const App::DocumentObject*) const;

protected:
    void changeEvent(QEvent* e) override;

private:
    void onSwapButtonClicked();
    void currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

private:
    std::unique_ptr<Ui_DlgBooleanOperation> ui;
    Connection connectNewObject;
    Connection connectModObject;
    std::list<const App::DocumentObject*> observe;
};

class TaskBooleanOperation: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskBooleanOperation();

public:
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Apply | QDialogButtonBox::Close;
    }
    bool isAllowedAlterDocument() const override
    {
        return true;
    }
    bool needsFullSpace() const override
    {
        return true;
    }

private:
    DlgBooleanOperation* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGBOOLEANOPERATION_H
