#ifndef PARTGUI_MIRRORING_H
#define PARTGUI_MIRRORING_H

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

class Ui_Mirroring;
class Mirroring: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit Mirroring(QWidget* parent = nullptr);
    ~Mirroring() override;
    bool accept();
    bool reject();

protected:
    void changeEvent(QEvent* e) override;

private:
    void findShapes();
    void onSelectButtonClicked();

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    QString document;
    std::unique_ptr<Ui_Mirroring> ui;
};

class TaskMirroring: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskMirroring();

public:
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }
    bool isAllowedAlterDocument() const override
    {
        return false;
    }
    bool needsFullSpace() const override
    {
        return false;
    }

private:
    Mirroring* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_MIRRORING_H
