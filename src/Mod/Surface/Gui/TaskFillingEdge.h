#ifndef SURFACEGUI_TASKFILLINGEDGE_H
#define SURFACEGUI_TASKFILLINGEDGE_H

#include <Gui/DocumentObserver.h>
#include <Gui/Selection/SelectionFilter.h>
#include <Mod/Surface/App/FeatureFilling.h>
#include <Mod/Surface/Gui/SelectionMode.h>
#include <QWidget>


class QListWidgetItem;

namespace Gui
{
class ButtonGroup;
}

namespace SurfaceGui
{

class ViewProviderFilling;
class Ui_TaskFillingEdge;

class FillingEdgePanel: public QWidget, public Gui::SelectionObserver, public Gui::DocumentObserver
{
    Q_OBJECT

protected:
    class ShapeSelection;
    enum SelectionMode
    {
        None = SurfaceGui::SelectionMode::None,
        AppendEdge = SurfaceGui::SelectionMode::AppendEdgeConstraint,
        RemoveEdge = SurfaceGui::SelectionMode::RemoveEdgeConstraint
    };
    SelectionMode selectionMode;
    Surface::Filling* editedObject;
    bool checkCommand;

private:
    Ui_TaskFillingEdge* ui;
    ViewProviderFilling* vp;

public:
    FillingEdgePanel(ViewProviderFilling* vp, Surface::Filling* obj);
    ~FillingEdgePanel() override;

    void open();
    void checkOpenCommand();
    bool accept();
    bool reject();
    void setEditedObject(Surface::Filling* obj);
    void appendButtons(Gui::ButtonGroup*);

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    /** Notifies on undo */
    void slotUndoDocument(const Gui::Document& Doc) override;
    /** Notifies on redo */
    void slotRedoDocument(const Gui::Document& Doc) override;
    /** Notifies when the object is about to be removed. */
    void slotDeletedObject(const Gui::ViewProviderDocumentObject& Obj) override;
    void modifyBoundary(bool);

private:
    void setupConnections();
    void onButtonUnboundEdgeAddToggled(bool checked);
    void onButtonUnboundEdgeRemoveToggled(bool checked);
    void onListUnboundItemDoubleClicked(QListWidgetItem*);
    void onButtonUnboundAcceptClicked();
    void onButtonUnboundIgnoreClicked();
    void onDeleteUnboundEdge();
    void clearSelection();

private:
    void exitSelectionMode();
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_TASKFILLINGEDGE_H
