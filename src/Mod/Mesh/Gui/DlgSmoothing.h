#ifndef MESHGUI_DLGSMOOTHING_H
#define MESHGUI_DLGSMOOTHING_H

#include <QDialog>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif

class QButtonGroup;

namespace MeshGui
{

class Selection;
class Ui_DlgSmoothing;
class DlgSmoothing: public QWidget
{
    Q_OBJECT

public:
    enum Smooth
    {
        None,
        Taubin,
        Laplace,
        MedianFilter
    };

    explicit DlgSmoothing(QWidget* parent = nullptr);
    ~DlgSmoothing() override;
    int iterations() const;
    double lambdaStep() const;
    double microStep() const;
    Smooth method() const;
    bool smoothSelection() const;

private:
    void methodClicked(int);
    void onCheckBoxSelectionToggled(bool);

Q_SIGNALS:
    void toggledSelection(bool);

private:
    Ui_DlgSmoothing* ui;
    QButtonGroup* bg;

    Q_DISABLE_COPY_MOVE(DlgSmoothing)
};

/**
 * Embed the panel into a dialog.
 */
class MeshGuiExport SmoothingDialog: public QDialog
{
    Q_OBJECT

public:
    explicit SmoothingDialog(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~SmoothingDialog() override;

    int iterations() const
    {
        return widget->iterations();
    }
    double lambdaStep() const
    {
        return widget->lambdaStep();
    }
    double microStep() const
    {
        return widget->microStep();
    }
    DlgSmoothing::Smooth method() const
    {
        return widget->method();
    }
    bool smoothSelection() const
    {
        return widget->smoothSelection();
    }

private:
    DlgSmoothing* widget;

    Q_DISABLE_COPY_MOVE(SmoothingDialog)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskSmoothing: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskSmoothing();

public:
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }
    bool isAllowedAlterDocument() const override
    {
        return true;
    }

private:
    DlgSmoothing* widget;
    Selection* selection;
};

}  // namespace MeshGui

#endif  // MESHGUI_DLGSMOOTHING_H
