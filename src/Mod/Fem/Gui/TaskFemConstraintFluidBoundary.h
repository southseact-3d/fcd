#ifndef GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H
#define GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H

#include <QObject>
#include <memory>

#include <Mod/Fem/App/FemSolverObject.h>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintFluidBoundary.h"


class Ui_TaskFemConstraintFluidBoundary;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace FemGui
{

class TaskFemConstraintFluidBoundary: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintFluidBoundary(
        ViewProviderFemConstraintFluidBoundary* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintFluidBoundary() override;

    const Fem::FemSolverObject* getFemSolver() const;

    std::string getBoundaryType() const;
    std::string getSubtype() const;
    double getBoundaryValue() const;

    std::string getTurbulenceModel() const;
    std::string getTurbulenceSpecification() const;
    double getTurbulentIntensityValue() const;
    double getTurbulentLengthValue() const;

    bool getHeatTransferring() const;
    std::string getThermalBoundaryType() const;
    double getTemperatureValue() const;
    double getHeatFluxValue() const;
    double getHTCoeffValue() const;

    const std::string getReferences() const override;
    const std::string getDirectionName() const;
    const std::string getDirectionObject() const;
    bool getReverse() const;

private Q_SLOTS:
    void onBoundaryTypeChanged();
    void onSubtypeChanged();
    void onBoundaryValueChanged(double);
    void onTurbulenceSpecificationChanged();
    void onThermalBoundaryTypeChanged();
    void onReferenceDeleted();
    void onButtonDirection(const bool pressed = true);
    void onCheckReverse(bool);  // consider removing this slot as the UI is hidden
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateBoundaryTypeUI();
    void updateSubtypeUI();
    void updateThermalBoundaryUI();
    void updateTurbulenceUI();
    void updateUI();

private:
    std::unique_ptr<Ui_TaskFemConstraintFluidBoundary> ui;
    int dimension;  // -1: unknown, 2 for 2D and 3 for 3D
    Fem::FemSolverObject* pcSolver;
    App::PropertyBool* pHeatTransferring;
    App::PropertyEnumeration* pTurbulenceModel;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintFluidBoundary: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintFluidBoundary(ViewProviderFemConstraintFluidBoundary* ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H
