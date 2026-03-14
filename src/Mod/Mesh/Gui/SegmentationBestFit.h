#ifndef MESHGUI_SEGMENTATIONBESTFIT_H
#define MESHGUI_SEGMENTATIONBESTFIT_H

#include <list>
#include <QDialog>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

#include <Mod/Mesh/MeshGlobal.h>
#include "MeshSelection.h"


class QDoubleSpinBox;

// forward declarations
namespace Mesh
{
class Feature;
}

namespace MeshGui
{
class Ui_SegmentationBestFit;

class FitParameter
{
public:
    struct Points
    {
        std::vector<Base::Vector3f> points;
        std::vector<Base::Vector3f> normals;
    };
    FitParameter() = default;
    virtual ~FitParameter() = default;
    FitParameter(const FitParameter&) = delete;
    FitParameter(FitParameter&&) = delete;
    FitParameter& operator=(const FitParameter&) = delete;
    FitParameter& operator=(FitParameter&&) = delete;
    virtual std::vector<float> getParameter(Points) const = 0;
};

using ParameterList = std::list<std::pair<QString, float>>;
class ParametersDialog: public QDialog
{
    Q_OBJECT

public:
    ParametersDialog(
        std::vector<float>&,
        FitParameter*,
        ParameterList,
        Mesh::Feature* mesh,
        QWidget* parent = nullptr
    );
    ~ParametersDialog() override;
    void accept() override;
    void reject() override;

private:
    void onRegionClicked();
    void onSingleClicked();
    void onClearClicked();
    void onComputeClicked();

private:
    std::vector<float>& values;
    FitParameter* fitParameter;
    ParameterList parameter;
    Mesh::Feature* myMesh;
    MeshSelection meshSel;
    std::vector<QDoubleSpinBox*> spinBoxes;

    Q_DISABLE_COPY_MOVE(ParametersDialog)
};

class MeshGuiExport SegmentationBestFit: public QWidget
{
    Q_OBJECT

public:
    explicit SegmentationBestFit(
        Mesh::Feature* mesh,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~SegmentationBestFit() override;
    void accept();

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();
    void onPlaneParametersClicked();
    void onCylinderParametersClicked();
    void onSphereParametersClicked();

private:
    std::vector<float> planeParameter;
    std::vector<float> cylinderParameter;
    std::vector<float> sphereParameter;
    Ui_SegmentationBestFit* ui;
    Mesh::Feature* myMesh;
    MeshSelection meshSel;

    Q_DISABLE_COPY_MOVE(SegmentationBestFit)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskSegmentationBestFit: public Gui::TaskView::TaskDialog
{
public:
    explicit TaskSegmentationBestFit(Mesh::Feature* mesh);

public:
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    SegmentationBestFit* widget;
};

}  // namespace MeshGui

#endif  // MESHGUI_SEGMENTATIONBESTFIT_H
