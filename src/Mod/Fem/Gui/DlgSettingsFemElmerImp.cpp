#include <QMessageBox>
#include <QStandardPaths>

#include "DlgSettingsFemElmerImp.h"
#include "ui_DlgSettingsFemElmer.h"


using namespace FemGui;

DlgSettingsFemElmerImp::DlgSettingsFemElmerImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemElmerImp)
{
    ui->setupUi(this);

    connect(
        ui->fc_grid_binary_path,
        &Gui::PrefFileChooser::fileNameSelected,
        this,
        &DlgSettingsFemElmerImp::onfileNameSelected
    );
    connect(
        ui->fc_elmer_binary_path,
        &Gui::PrefFileChooser::fileNameSelected,
        this,
        &DlgSettingsFemElmerImp::onfileNameSelected
    );
}

DlgSettingsFemElmerImp::~DlgSettingsFemElmerImp() = default;

void DlgSettingsFemElmerImp::saveSettings()
{
    ui->fc_elmer_binary_path->onSave();
    ui->fc_grid_binary_path->onSave();

    ui->sb_num_tasks->onSave();
    ui->sb_threads_per_task->onSave();

    ui->ckb_binary_format->onSave();
    ui->ckb_geom_id->onSave();
}

void DlgSettingsFemElmerImp::loadSettings()
{
    ui->fc_elmer_binary_path->onRestore();
    ui->fc_grid_binary_path->onRestore();

    ui->sb_num_tasks->onRestore();
    ui->sb_threads_per_task->onRestore();

    ui->ckb_binary_format->onRestore();
    ui->ckb_geom_id->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemElmerImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgSettingsFemElmerImp::onfileNameSelected(const QString& fileName)
{
    if (!fileName.isEmpty() && QStandardPaths::findExecutable(fileName).isEmpty()) {
        QMessageBox::critical(this, tr("Elmer"), tr("Executable '%1' not found").arg(fileName));
    }
}

#include "moc_DlgSettingsFemElmerImp.cpp"
