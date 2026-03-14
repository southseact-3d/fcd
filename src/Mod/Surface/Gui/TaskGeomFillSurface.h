#ifndef SURFACEGUI_TASKGEOMFILLSURFACE_H
#define SURFACEGUI_TASKGEOMFILLSURFACE_H

#include <GeomFill_FillingStyle.hxx>

#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Part/Gui/ViewProviderSpline.h>
#include <Mod/Surface/App/FeatureGeomFillSurface.h>


class QListWidgetItem;

namespace Gui
{
class ButtonGroup;
}

namespace SurfaceGui
{

class Ui_GeomFillSurface;

class ViewProviderGeomFillSurface: public PartGui::ViewProviderSpline
{
    PROPERTY_HEADER_WITH_OVERRIDE(SurfaceGui::ViewProviderGeomFillSurface);

public:
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    QIcon getIcon() const override;
    void highlightReferences(bool on);
};

class GeomFillSurface: public QWidget, public Gui::SelectionObserver, public Gui::DocumentObserver
{
    Q_OBJECT

protected:
    class EdgeSelection;
    enum SelectionMode
    {
        None,
        Append,
        Remove
    };
    SelectionMode selectionMode;
    Surface::GeomFillSurface* editedObject;
    bool checkCommand;

private:
    Ui_GeomFillSurface* ui;
    ViewProviderGeomFillSurface* vp;
    Gui::ButtonGroup* buttonGroup;

public:
    GeomFillSurface(ViewProviderGeomFillSurface* vp, Surface::GeomFillSurface* obj);
    ~GeomFillSurface() override;

    void open();
    void checkOpenCommand();
    bool accept();
    bool reject();
    void setEditedObject(Surface::GeomFillSurface* obj);

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    /** Notifies on undo */
    void slotUndoDocument(const Gui::Document& Doc) override;
    /** Notifies on redo */
    void slotRedoDocument(const Gui::Document& Doc) override;
    /** Notifies when the object is about to be removed. */
    void slotDeletedObject(const Gui::ViewProviderDocumentObject& Obj) override;
    void changeFillType(GeomFill_FillingStyle);
    void flipOrientation(QListWidgetItem*);

private:
    void setupConnections();
    void onFillTypeStretchClicked();
    void onFillTypeCoonsClicked();
    void onFillTypeCurvedClicked();
    void onButtonEdgeAddToggled(bool checked);
    void onButtonEdgeRemoveToggled(bool checked);
    void onListWidgetItemDoubleClicked(QListWidgetItem*);
    void onDeleteEdge();
    void onFlipOrientation();
    void clearSelection();

private:
    void exitSelectionMode();
};

class TaskGeomFillSurface: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskGeomFillSurface(ViewProviderGeomFillSurface* vp, Surface::GeomFillSurface* obj);
    void setEditedObject(Surface::GeomFillSurface* obj);

public:
    void open() override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    GeomFillSurface* widget;
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_TASKGEOMFILLSURFACE_H
