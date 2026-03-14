#ifndef GUI_TASKELEMENTCOLORS_H
#define GUI_TASKELEMENTCOLORS_H

#include <QListWidgetItem>
#include "TaskView/TaskDialog.h"
#include "TaskView/TaskView.h"


namespace Gui
{
class Document;
class ViewProvider;
class ViewProviderDocumentObject;

class GuiExport ElementColors: public QWidget, public SelectionObserver
{
    Q_OBJECT

public:
    explicit ElementColors(ViewProviderDocumentObject* vp, bool noHide = false);
    ~ElementColors() override;

    bool accept();
    bool reject();

private:
    void setupConnections();
    void onRemoveSelectionClicked();
    void onAddSelectionClicked();
    void onRemoveAllClicked();
    void onElementListItemDoubleClicked(QListWidgetItem* item);
    void onElementListItemSelectionChanged();
    void onElementListItemEntered(QListWidgetItem* item);
    void onRecomputeClicked(bool checked);
    void onTopClicked(bool checked);
    void onHideSelectionClicked();
    void onBoxSelectClicked();

protected:
    void onSelectionChanged(const SelectionChanges& msg) override;
    void changeEvent(QEvent* e) override;
    void leaveEvent(QEvent*) override;
    void slotDeleteDocument(const Document&);
    void slotDeleteObject(const ViewProvider&);

private:
    class Private;
    Private* d;
};

class GuiExport TaskElementColors: public TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskElementColors(ViewProviderDocumentObject* vp, bool noHide = false);
    ~TaskElementColors() override;

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
    ElementColors* widget;
};

}  // namespace Gui

#endif  // GUI_TASKELEMENTCOLORS_H
