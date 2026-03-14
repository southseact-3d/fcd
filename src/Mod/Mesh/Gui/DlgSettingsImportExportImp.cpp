#include <App/Application.h>
#include <Mod/Mesh/App/Core/MeshIO.h>

#include "DlgSettingsImportExportImp.h"
#include "ui_DlgSettingsImportExport.h"


using namespace MeshGui;

DlgSettingsImportExport::DlgSettingsImportExport(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsImportExport)
{
    ui->setupUi(this);
    ui->exportAmfCompressed->setToolTip(
        tr("This parameter indicates whether ZIP compression\n"
           "is used when writing a file in AMF format")
    );
}

DlgSettingsImportExport::~DlgSettingsImportExport()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void DlgSettingsImportExport::saveSettings()
{
    ParameterGrp::handle handle = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Mesh"
    );
    double value = ui->maxDeviationExport->value().getValue();
    handle->SetFloat("MaxDeviationExport", value);

    ui->exportAmfCompressed->onSave();
    ui->export3mfModel->onSave();

    ParameterGrp::handle asy = handle->GetGroup("Asymptote");
    asy->SetASCII("Width", ui->asymptoteWidth->text().toLatin1());
    asy->SetASCII("Height", ui->asymptoteHeight->text().toLatin1());

    MeshCore::MeshOutput::SetAsymptoteSize(
        ui->asymptoteWidth->text().toStdString(),
        ui->asymptoteHeight->text().toStdString()
    );
}

void DlgSettingsImportExport::loadSettings()
{
    ParameterGrp::handle handle = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Mesh"
    );
    double value = ui->maxDeviationExport->value().getValue();
    value = handle->GetFloat("MaxDeviationExport", value);
    ui->maxDeviationExport->setValue(value);

    ui->exportAmfCompressed->onRestore();
    ui->export3mfModel->onRestore();

    ParameterGrp::handle asy = handle->GetGroup("Asymptote");
    ui->asymptoteWidth->setText(QString::fromStdString(asy->GetASCII("Width")));
    ui->asymptoteHeight->setText(QString::fromStdString(asy->GetASCII("Height")));
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsImportExport::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        PreferencePage::changeEvent(e);
    }
}

#include "moc_DlgSettingsImportExportImp.cpp"
