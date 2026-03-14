#include <QMessageBox>
#include <QStandardPaths>

#include <Gui/Application.h>

#include "DlgSettingsFemZ88Imp.h"
#include "ui_DlgSettingsFemZ88.h"


using namespace FemGui;

DlgSettingsFemZ88Imp::DlgSettingsFemZ88Imp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemZ88Imp)
{
    ui->setupUi(this);

    connect(
        ui->fc_z88_binary_path,
        &Gui::PrefFileChooser::fileNameSelected,
        this,
        &DlgSettingsFemZ88Imp::onfileNameSelected
    );
}

DlgSettingsFemZ88Imp::~DlgSettingsFemZ88Imp() = default;

void DlgSettingsFemZ88Imp::saveSettings()
{
    ui->fc_z88_binary_path->onSave();

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/Z88"
    );
    hGrp->SetInt("Solver", ui->cmb_solver->currentIndex());
    ui->cmb_solver->onSave();
    hGrp->SetInt("MaxGS", ui->sb_Z88_MaxGS->value());
    ui->sb_Z88_MaxGS->onSave();
    hGrp->SetInt("MaxKOI", ui->sb_Z88_MaxKOI->value());
    ui->sb_Z88_MaxKOI->onSave();
}

void DlgSettingsFemZ88Imp::loadSettings()
{
    ui->fc_z88_binary_path->onRestore();
    ui->cmb_solver->onRestore();
    ui->sb_Z88_MaxGS->onRestore();

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/Z88"
    );
    int index = hGrp->GetInt("Solver", 0);
    if (index > -1) {
        ui->cmb_solver->setCurrentIndex(index);
    }
    int places = hGrp->GetInt("MaxGS", 100000000);
    if (places > -1) {
        ui->sb_Z88_MaxGS->setValue(places);
    }
    places = hGrp->GetInt("MaxKOI", 2800000);
    if (places > -1) {
        ui->sb_Z88_MaxKOI->setValue(places);
    }
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemZ88Imp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgSettingsFemZ88Imp::onfileNameSelected(const QString& fileName)
{
    if (!fileName.isEmpty() && QStandardPaths::findExecutable(fileName).isEmpty()) {
        QMessageBox::critical(this, tr("Z88"), tr("Executable '%1' not found").arg(fileName));
    }
}

#include "moc_DlgSettingsFemZ88Imp.cpp"
