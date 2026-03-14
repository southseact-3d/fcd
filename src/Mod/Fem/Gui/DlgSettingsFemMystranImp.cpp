#include <QMessageBox>
#include <QStandardPaths>

#include "DlgSettingsFemMystranImp.h"
#include "ui_DlgSettingsFemMystran.h"


using namespace FemGui;

DlgSettingsFemMystranImp::DlgSettingsFemMystranImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemMystranImp)
{
    ui->setupUi(this);

    connect(
        ui->fc_mystran_binary_path,
        &Gui::PrefFileChooser::fileNameSelected,
        this,
        &DlgSettingsFemMystranImp::onfileNameSelected
    );
}

DlgSettingsFemMystranImp::~DlgSettingsFemMystranImp() = default;

void DlgSettingsFemMystranImp::saveSettings()
{
    ui->fc_mystran_binary_path->onSave();
    ui->cb_mystran_write_comments->onSave();
}

void DlgSettingsFemMystranImp::loadSettings()
{
    ui->fc_mystran_binary_path->onRestore();
    ui->cb_mystran_write_comments->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemMystranImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgSettingsFemMystranImp::onfileNameSelected(const QString& fileName)
{
    if (!fileName.isEmpty() && QStandardPaths::findExecutable(fileName).isEmpty()) {
        QMessageBox::critical(this, tr("Mystran"), tr("Executable '%1' not found").arg(fileName));
    }
}

#include "moc_DlgSettingsFemMystranImp.cpp"
