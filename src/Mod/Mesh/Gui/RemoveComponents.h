#ifndef MESHGUI_REMOVECOMPONENTS_H
#define MESHGUI_REMOVECOMPONENTS_H

#include <QDialog>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Mesh/MeshGlobal.h>
#include "MeshSelection.h"

namespace MeshGui
{
class Ui_RemoveComponents;

/**
 * Non-modal dialog to de/select components, regions, the complete or single faces
 * of a mesh and delete them.
 * @author Werner Mayer
 */
class MeshGuiExport RemoveComponents: public QWidget
{
    Q_OBJECT

public:
    explicit RemoveComponents(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~RemoveComponents() override;
    void reject();
    void deleteSelection();
    void invertSelection();

public:
    void onSelectRegionClicked();
    void onSelectAllClicked();
    void onSelectComponentsClicked();
    void onSelectTriangleClicked();
    void onDeselectRegionClicked();
    void onDeselectAllClicked();
    void onDeselectComponentsClicked();
    void onDeselectTriangleClicked();
    void onVisibleTrianglesToggled(bool);
    void onScreenTrianglesToggled(bool);
    void onSelectCompToggled(bool);
    void onDeselectCompToggled(bool);

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();

private:
    Ui_RemoveComponents* ui;
    MeshSelection meshSel;

    Q_DISABLE_COPY_MOVE(RemoveComponents)
};

/**
 * Embed the panel into a dialog.
 */
class MeshGuiExport RemoveComponentsDialog: public QDialog
{
    Q_OBJECT

public:
    explicit RemoveComponentsDialog(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~RemoveComponentsDialog() override;
    void reject() override;

private Q_SLOTS:
    void clicked(QAbstractButton* btn);

private:
    RemoveComponents* widget;

    Q_DISABLE_COPY_MOVE(RemoveComponentsDialog)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskRemoveComponents: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskRemoveComponents();

public:
    bool accept() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Close;
    }
    bool isAllowedAlterDocument() const override
    {
        return true;
    }
    void modifyStandardButtons(QDialogButtonBox*) override;

private:
    RemoveComponents* widget;
};

}  // namespace MeshGui

#endif  // MESHGUI_REMOVECOMPONENTS_H
