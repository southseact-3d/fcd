#include <App/Application.h>
#include <Mod/Fem/App/FemTools.h>

#include "DlgSettingsFemGeneralImp.h"
#include "ui_DlgSettingsFemGeneral.h"


using namespace FemGui;

DlgSettingsFemGeneralImp::DlgSettingsFemGeneralImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemGeneralImp)
{
    ui->setupUi(this);

    // fill solvers combo with available solvers
    ui->cmb_def_solver->clear();
    std::vector<std::string> Solvers = {"None"};

    if (!Fem::Tools::checkIfBinaryExists("Ccx", "ccx", "ccx").empty()) {
        Solvers.emplace_back("CalculiX");
    }
    if (!Fem::Tools::checkIfBinaryExists("Elmer", "elmer", "ElmerSolver").empty()) {
        Solvers.emplace_back("Elmer");
    }
    // also check the multi-CPU Elmer build
    else if (!Fem::Tools::checkIfBinaryExists("Elmer", "elmer", "ElmerSolver_mpi").empty()) {
        Solvers.emplace_back("Elmer");
    }
    if (!Fem::Tools::checkIfBinaryExists("Mystran", "mystran", "mystran").empty()) {
        Solvers.emplace_back("Mystran");
    }
    if (!Fem::Tools::checkIfBinaryExists("Z88", "z88", "z88r").empty()) {
        Solvers.emplace_back("Z88");
    }

    QStringList solversList;
    for (auto item : Solvers) {
        solversList << QLatin1String(item.c_str());
    }
    ui->cmb_def_solver->addItems(solversList);

    // if the "DefaultSolver" parameter is not yet set and there is only
    // one available solver, set this solver
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/General"
    );
    auto DefaultSolver = hGrp->GetInt("DefaultSolver", 0);
    if (!DefaultSolver && ui->cmb_def_solver->count() == 2) {
        ui->cmb_def_solver->setCurrentIndex(1);
    }
}

DlgSettingsFemGeneralImp::~DlgSettingsFemGeneralImp() = default;

void DlgSettingsFemGeneralImp::saveSettings()
{
    ui->cb_analysis_group_meshing->onSave();

    ui->cb_restore_result_dialog->onSave();
    ui->cb_keep_results_on_rerun->onSave();
    ui->cb_hide_constraint->onSave();

    ui->cb_wd_temp->onSave();
    ui->cb_wd_beside->onSave();
    ui->cb_wd_custom->onSave();
    ui->le_wd_custom->onSave();
    ui->cb_overwrite_solver_working_directory->onSave();
    ui->cmb_def_solver->onSave();
}

void DlgSettingsFemGeneralImp::loadSettings()
{
    ui->cb_analysis_group_meshing->onRestore();

    ui->cb_restore_result_dialog->onRestore();
    ui->cb_keep_results_on_rerun->onRestore();
    ui->cb_hide_constraint->onRestore();

    ui->cb_wd_temp->onRestore();
    ui->cb_wd_beside->onRestore();
    ui->cb_wd_custom->onRestore();
    ui->le_wd_custom->onRestore();
    ui->cb_overwrite_solver_working_directory->onRestore();
    ui->cmb_def_solver->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemGeneralImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsFemGeneralImp.cpp"
