#ifndef GUI_TASKVIEW_TaskFemConstraintHeatflux_H
#define GUI_TASKVIEW_TaskFemConstraintHeatflux_H

#include <QObject>
#include <memory>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintHeatflux.h"


class Ui_TaskFemConstraintHeatflux;

namespace FemGui
{
class TaskFemConstraintHeatflux: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintHeatflux(
        ViewProviderFemConstraintHeatflux* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintHeatflux() override;
    std::string getAmbientTemp() const;
    /*double getFaceTemp(void) const;*/
    std::string getFilmCoef() const;
    std::string getDFlux() const;
    double getEmissivity() const;
    std::string getConstraintType() const;
    const std::string getReferences() const override;

private Q_SLOTS:
    void onReferenceDeleted();
    void onAmbientTempChanged(double val);
    /*void onFaceTempChanged(double val);*/
    void onFilmCoefChanged(double val);
    void onEmissivityChanged(double val);
    void onHeatFluxChanged(double val);
    void onConstrTypeChanged(int val);
    void Conv();
    void Rad();
    void Flux();
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintHeatflux> ui;
};

class TaskDlgFemConstraintHeatflux: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintHeatflux(ViewProviderFemConstraintHeatflux* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintHeatflux_H
