#include <QMessageBox>

#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Mod/Fem/App/FemConstraintPulley.h>

#include "TaskFemConstraintPulley.h"
#include "ui_TaskFemConstraintBearing.h"


using namespace FemGui;
using namespace Gui;

/* TRANSLATOR FemGui::TaskFemConstraintPulley */

TaskFemConstraintPulley::TaskFemConstraintPulley(
    ViewProviderFemConstraintPulley* ConstraintView,
    QWidget* parent
)
    : TaskFemConstraintGear(ConstraintView, parent, "FEM_ConstraintPulley")
{
    connect(
        ui->spinOtherDiameter,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        &TaskFemConstraintPulley::onOtherDiameterChanged
    );
    connect(
        ui->spinCenterDistance,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        &TaskFemConstraintPulley::onCenterDistanceChanged
    );
    connect(ui->checkIsDriven, &QCheckBox::toggled, this, &TaskFemConstraintPulley::onCheckIsDriven);
    connect(
        ui->spinTensionForce,
        qOverload<double>(&QDoubleSpinBox::valueChanged),
        this,
        &TaskFemConstraintPulley::onTensionForceChanged
    );

    // Temporarily prevent unnecessary feature recomputes
    ui->spinOtherDiameter->blockSignals(true);
    ui->spinCenterDistance->blockSignals(true);
    ui->checkIsDriven->blockSignals(true);
    ui->spinTensionForce->blockSignals(true);

    // Get the feature data
    Fem::ConstraintPulley* pcConstraint = ConstraintView->getObject<Fem::ConstraintPulley>();
    double otherdia = pcConstraint->OtherDiameter.getValue();
    double centerdist = pcConstraint->CenterDistance.getValue();
    bool isdriven = pcConstraint->IsDriven.getValue();
    double tensionforce = pcConstraint->TensionForce.getValue();

    // Fill data into dialog elements
    ui->spinOtherDiameter->setMinimum(0);
    ui->spinOtherDiameter->setMaximum(std::numeric_limits<float>::max());
    ui->spinOtherDiameter->setValue(otherdia);
    ui->spinCenterDistance->setMinimum(0);
    ui->spinCenterDistance->setMaximum(std::numeric_limits<float>::max());
    ui->spinCenterDistance->setValue(centerdist);
    ui->checkIsDriven->setChecked(isdriven);
    ui->spinForce->setMinimum(-std::numeric_limits<float>::max());
    ui->spinTensionForce->setMinimum(0);
    ui->spinTensionForce->setMaximum(std::numeric_limits<float>::max());
    ui->spinTensionForce->setValue(tensionforce);

    // Adjust ui
    ui->buttonDirection->setVisible(false);
    ui->lineDirection->setVisible(false);
    ui->checkReversed->setVisible(false);
    ui->labelDiameter->setText(tr("Pulley diameter"));
    ui->labelForce->setText(tr("Torque [Nm]"));
    ui->labelOtherDiameter->setVisible(true);
    ui->spinOtherDiameter->setVisible(true);
    ui->labelCenterDistance->setVisible(true);
    ui->spinCenterDistance->setVisible(true);
    ui->checkIsDriven->setVisible(true);
    ui->labelTensionForce->setVisible(true);
    ui->spinTensionForce->setVisible(true);

    ui->spinOtherDiameter->blockSignals(false);
    ui->spinCenterDistance->blockSignals(false);
    ui->checkIsDriven->blockSignals(false);
    ui->spinTensionForce->blockSignals(false);
}

void TaskFemConstraintPulley::onOtherDiameterChanged(double l)
{
    Fem::ConstraintPulley* pcConstraint = ConstraintView->getObject<Fem::ConstraintPulley>();
    pcConstraint->OtherDiameter.setValue(l);
}

void TaskFemConstraintPulley::onCenterDistanceChanged(double l)
{
    Fem::ConstraintPulley* pcConstraint = ConstraintView->getObject<Fem::ConstraintPulley>();
    pcConstraint->CenterDistance.setValue(l);
}

void TaskFemConstraintPulley::onTensionForceChanged(double force)
{
    Fem::ConstraintPulley* pcConstraint = ConstraintView->getObject<Fem::ConstraintPulley>();
    pcConstraint->TensionForce.setValue(force);
}

void TaskFemConstraintPulley::onCheckIsDriven(const bool pressed)
{
    Fem::ConstraintPulley* pcConstraint = ConstraintView->getObject<Fem::ConstraintPulley>();
    pcConstraint->IsDriven.setValue(pressed);
}

double TaskFemConstraintPulley::getTorque() const
{
    return ui->spinForce->value();
}

double TaskFemConstraintPulley::getTensionForce() const
{
    return ui->spinTensionForce->value();
}

bool TaskFemConstraintPulley::getIsDriven() const
{
    return ui->checkIsDriven->isChecked();
}

double TaskFemConstraintPulley::getOtherDiameter() const
{
    return ui->spinOtherDiameter->value();
}

double TaskFemConstraintPulley::getCenterDistance() const
{
    return ui->spinCenterDistance->value();
}

void TaskFemConstraintPulley::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->spinOtherDiameter->blockSignals(true);
        ui->spinCenterDistance->blockSignals(true);
        ui->checkIsDriven->blockSignals(true);
        ui->spinTensionForce->blockSignals(true);
        ui->retranslateUi(proxy);
        ui->spinOtherDiameter->blockSignals(false);
        ui->spinCenterDistance->blockSignals(false);
        ui->checkIsDriven->blockSignals(false);
        ui->spinTensionForce->blockSignals(false);
    }
}

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgFemConstraintPulley::TaskDlgFemConstraintPulley(ViewProviderFemConstraintPulley* ConstraintView)
{
    this->ConstraintView = ConstraintView;
    assert(ConstraintView);
    this->parameter = new TaskFemConstraintPulley(ConstraintView);

    Content.push_back(parameter);
}

//==== calls from the TaskView ===============================================================

bool TaskDlgFemConstraintPulley::accept()
{
    std::string name = ConstraintView->getObject()->getNameInDocument();
    const TaskFemConstraintPulley* parameterPulley = static_cast<const TaskFemConstraintPulley*>(
        parameter
    );

    try {
        // Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "FEM pulley constraint changed"));
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.OtherDiameter = %f",
            name.c_str(),
            parameterPulley->getOtherDiameter()
        );
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.CenterDistance = %f",
            name.c_str(),
            parameterPulley->getCenterDistance()
        );
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.IsDriven = %s",
            name.c_str(),
            parameterPulley->getIsDriven() ? "True" : "False"
        );
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.TensionForce = %f",
            name.c_str(),
            parameterPulley->getTensionForce()
        );
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(parameter, tr("Input error"), QString::fromLatin1(e.what()));
        return false;
    }

    return TaskDlgFemConstraintGear::accept();
}

#include "moc_TaskFemConstraintPulley.cpp"
