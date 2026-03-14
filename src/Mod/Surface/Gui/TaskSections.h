#ifndef SURFACEGUI_TASKSECTIONS_H
#define SURFACEGUI_TASKSECTIONS_H

#include <memory>

#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Part/Gui/ViewProviderSpline.h>
#include <Mod/Surface/App/FeatureSections.h>


class QListWidgetItem;

namespace Gui
{
class ButtonGroup;
}

namespace SurfaceGui
{

class Ui_Sections;

class ViewProviderSections: public PartGui::ViewProviderSpline
{
    PROPERTY_HEADER_WITH_OVERRIDE(SurfaceGui::ViewProviderSections);
    using References = std::vector<App::PropertyLinkSubList::SubSet>;

public:
    enum ShapeType
    {
        Vertex,
        Edge,
        Face
    };
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    QIcon getIcon() const override;
    void highlightReferences(ShapeType type, const References& refs, bool on);
};

class SectionsPanel: public QWidget, public Gui::SelectionObserver, public Gui::DocumentObserver
{
    Q_OBJECT

protected:
    class ShapeSelection;
    enum SelectionMode
    {
        None,
        AppendEdge,
        RemoveEdge
    };
    SelectionMode selectionMode;
    Surface::Sections* editedObject;
    bool checkCommand;

private:
    std::unique_ptr<Ui_Sections> ui;
    ViewProviderSections* vp;
    Gui::ButtonGroup* buttonGroup;

public:
    SectionsPanel(ViewProviderSections* vp, Surface::Sections* obj);
    ~SectionsPanel() override;

    void open();
    void checkOpenCommand();
    bool accept();
    bool reject();
    void setEditedObject(Surface::Sections* obj);

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
    void onButtonEdgeAddToggled(bool checked);
    void onButtonEdgeRemoveToggled(bool checked);
    void onDeleteEdge();
    void clearSelection();
    void onIndexesMoved();

    void appendCurve(App::DocumentObject*, const std::string& subname);
    void removeCurve(App::DocumentObject*, const std::string& subname);

    void exitSelectionMode();
};

class TaskSections: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskSections(ViewProviderSections* vp, Surface::Sections* obj);
    void setEditedObject(Surface::Sections* obj);

public:
    void open() override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    SectionsPanel* widget1;
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_TASKSECTIONS_H
