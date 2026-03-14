#ifndef MESHGUI_SEGMENTATION_H
#define MESHGUI_SEGMENTATION_H

#include <QWidget>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif


// forward declarations
namespace Mesh
{
class Feature;
}

namespace MeshGui
{
class Ui_Segmentation;

class MeshGuiExport Segmentation: public QWidget
{
public:
    explicit Segmentation(
        Mesh::Feature* mesh,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~Segmentation() override;
    void accept();

protected:
    void changeEvent(QEvent* e) override;

private:
    Ui_Segmentation* ui;
    Mesh::Feature* myMesh;

    Q_DISABLE_COPY_MOVE(Segmentation)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskSegmentation: public Gui::TaskView::TaskDialog
{
public:
    explicit TaskSegmentation(Mesh::Feature* mesh);

public:
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    Segmentation* widget;
};

}  // namespace MeshGui

#endif  // MESHGUI_SEGMENTATION_H
