#ifndef MESHGUI_DLGDECIMATING_H
#define MESHGUI_DLGDECIMATING_H

#include <Mod/Mesh/MeshGlobal.h>

#include <QDialog>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <memory>

namespace MeshGui
{
class Ui_DlgDecimating;
class DlgDecimating: public QWidget
{
    Q_OBJECT

public:
    explicit DlgDecimating(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgDecimating() override;
    void setNumberOfTriangles(int);
    double tolerance() const;
    double reduction() const;
    bool isAbsoluteNumber() const;
    int targetNumberOfTriangles() const;

private:
    void onCheckAbsoluteNumberToggled(bool);

private:
    int numberOfTriangles {0};
    std::unique_ptr<Ui_DlgDecimating> ui;

    Q_DISABLE_COPY_MOVE(DlgDecimating)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskDecimating: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDecimating();

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
    DlgDecimating* widget;
};

}  // namespace MeshGui

#endif  // MESHGUI_DLGDECIMATING_H
