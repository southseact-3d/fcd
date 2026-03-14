#include <Gui/Application.h>

#include "DlgSettingsFemInOutVtkImp.h"
#include "ui_DlgSettingsFemInOutVtk.h"


using namespace FemGui;

DlgSettingsFemInOutVtkImp::DlgSettingsFemInOutVtkImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemInOutVtk)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsFemInOutVtkImp::~DlgSettingsFemInOutVtkImp() = default;

void DlgSettingsFemInOutVtkImp::saveSettings()
{
    ui->comboBoxVtkImportObject->onSave();
    ui->cb_export_level->onSave();
}

void DlgSettingsFemInOutVtkImp::loadSettings()
{
    ui->comboBoxVtkImportObject->onRestore();

    populateExportLevel();
    ui->cb_export_level->onSave();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemInOutVtkImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        int c_index = ui->comboBoxVtkImportObject->currentIndex();
        ui->retranslateUi(this);
        ui->comboBoxVtkImportObject->setCurrentIndex(c_index);
        populateExportLevel();
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgSettingsFemInOutVtkImp::populateExportLevel() const
{
    std::list<std::string> values = {QT_TR_NOOP("All"), QT_TR_NOOP("Highest")};

    ui->cb_export_level->clear();
    for (const auto& val : values) {
        ui->cb_export_level->addItem(tr(val.c_str()), QByteArray::fromStdString(val));
    }

    // set default index
    auto hGrp = ui->cb_export_level->getWindowParameter();
    std::string current = hGrp->GetASCII(ui->cb_export_level->entryName(), "Highest");
    int index = ui->cb_export_level->findData(QByteArray::fromStdString(current));
    ui->cb_export_level->setCurrentIndex(index);
}

#include "moc_DlgSettingsFemInOutVtkImp.cpp"
