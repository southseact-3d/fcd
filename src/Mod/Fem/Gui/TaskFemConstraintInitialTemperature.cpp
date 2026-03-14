#include <QMessageBox>
#include <sstream>


#include <Gui/Command.h>
#include <Mod/Fem/App/FemConstraintInitialTemperature.h>

#include "TaskFemConstraintInitialTemperature.h"
#include "ui_TaskFemConstraintInitialTemperature.h"


using namespace FemGui;
using namespace Gui;

/* TRANSLATOR FemGui::TaskFemConstraintInitialTemperature */

TaskFemConstraintInitialTemperature::TaskFemConstraintInitialTemperature(
    ViewProviderFemConstraintInitialTemperature* ConstraintView,
    QWidget* parent
)
    : TaskFemConstraint(ConstraintView, parent, "FEM_ConstraintInitialTemperature")
    , ui(new Ui_TaskFemConstraintInitialTemperature)
{
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    // Get the feature data
    Fem::ConstraintInitialTemperature* pcConstraint
        = ConstraintView->getObject<Fem::ConstraintInitialTemperature>();
    std::vector<App::DocumentObject*> Objects = pcConstraint->References.getValues();
    std::vector<std::string> SubElements = pcConstraint->References.getSubValues();

    // Fill data into dialog elements
    ui->if_temperature->setValue(pcConstraint->initialTemperature.getQuantityValue());

    ui->if_temperature->bind(pcConstraint->initialTemperature);
}

TaskFemConstraintInitialTemperature::~TaskFemConstraintInitialTemperature() = default;

std::string TaskFemConstraintInitialTemperature::get_temperature() const
{
    return ui->if_temperature->value().getSafeUserString();
}

void TaskFemConstraintInitialTemperature::changeEvent(QEvent*)
{}


//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgFemConstraintInitialTemperature::TaskDlgFemConstraintInitialTemperature(
    ViewProviderFemConstraintInitialTemperature* ConstraintView
)
{
    this->ConstraintView = ConstraintView;
    assert(ConstraintView);
    this->parameter = new TaskFemConstraintInitialTemperature(ConstraintView);

    Content.push_back(parameter);
}

//==== calls from the TaskView ===============================================================

bool TaskDlgFemConstraintInitialTemperature::accept()
{
    std::string name = ConstraintView->getObject()->getNameInDocument();
    const TaskFemConstraintInitialTemperature* parameterTemperature
        = static_cast<const TaskFemConstraintInitialTemperature*>(parameter);

    try {
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.initialTemperature = \"%s\"",
            name.c_str(),
            parameterTemperature->get_temperature().c_str()
        );
        Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
        if (!ConstraintView->getObject()->isValid()) {
            throw Base::RuntimeError(ConstraintView->getObject()->getStatusString());
        }
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");
        Gui::Command::commitCommand();
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(parameter, tr("Input error"), QString::fromLatin1(e.what()));
        return false;
    }

    return true;
}

#include "moc_TaskFemConstraintInitialTemperature.cpp"
