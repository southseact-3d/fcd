#ifndef SURFACEGUI_TASKFILLINGVERTEX_H
#define SURFACEGUI_TASKFILLINGVERTEX_H

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
class Ui_TaskFillingVertex;

class FillingVertexPanel: public QWidget, public Gui::SelectionObserver, public Gui::DocumentObserver
{
    Q_OBJECT

protected:
    class VertexSelection;
    enum SelectionMode
    {
        None = SurfaceGui::SelectionMode::None,
        AppendVertex = SurfaceGui::SelectionMode::AppendVertexConstraint,
        RemoveVertex = SurfaceGui::SelectionMode::RemoveVertexConstraint
    };
    SelectionMode selectionMode;
    Surface::Filling* editedObject;
    bool checkCommand;

private:
    Ui_TaskFillingVertex* ui;
    ViewProviderFilling* vp;

public:
    FillingVertexPanel(ViewProviderFilling* vp, Surface::Filling* obj);
    ~FillingVertexPanel() override;

    void open();
    void reject();
    void checkOpenCommand();
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

private:
    void setupConnections();
    void onButtonVertexAddToggled(bool checked);
    void onButtonVertexRemoveToggled(bool checked);
    void onDeleteVertex();
    void clearSelection();

private:
    void exitSelectionMode();
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_TASKFILLINGVERTEX_H
