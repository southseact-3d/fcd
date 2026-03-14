#include <Gui/Application.h>

#include "DlgSettingsFemExportAbaqusImp.h"
#include "ui_DlgSettingsFemExportAbaqus.h"


using namespace FemGui;

DlgSettingsFemExportAbaqusImp::DlgSettingsFemExportAbaqusImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemExportAbaqus)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsFemExportAbaqusImp::~DlgSettingsFemExportAbaqusImp() = default;

void DlgSettingsFemExportAbaqusImp::saveSettings()
{
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/Abaqus"
    );
    hGrp->SetInt("AbaqusElementChoice", ui->comboBoxElemChoiceParam->currentIndex());

    ui->comboBoxElemChoiceParam->onSave();
    ui->checkBoxWriteGroups->onSave();
}

void DlgSettingsFemExportAbaqusImp::loadSettings()
{
    ui->comboBoxElemChoiceParam->onRestore();
    ui->checkBoxWriteGroups->onRestore();

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/Abaqus"
    );
    int index = hGrp->GetInt("AbaqusElementChoice", 2);
    if (index > -1) {
        ui->comboBoxElemChoiceParam->setCurrentIndex(index);
    }
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemExportAbaqusImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        int c_index = ui->comboBoxElemChoiceParam->currentIndex();
        ui->retranslateUi(this);
        ui->comboBoxElemChoiceParam->setCurrentIndex(c_index);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsFemExportAbaqusImp.cpp"
