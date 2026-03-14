#ifndef REVERSEENGINEERINGGUI_SEGMENTATION_H
#define REVERSEENGINEERINGGUI_SEGMENTATION_H

#include <memory>

#include <QWidget>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


// forward declarations
namespace Mesh
{
class Feature;
}

namespace ReverseEngineeringGui
{
class Ui_Segmentation;

class Segmentation: public QWidget
{
    Q_OBJECT

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
    std::unique_ptr<Ui_Segmentation> ui;
    App::DocumentObjectWeakPtrT myMesh;
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

}  // namespace ReverseEngineeringGui

#endif  // REVERSEENGINEERINGGUI_SEGMENTATION_H
