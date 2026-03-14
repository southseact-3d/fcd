#include <Gui/Application.h>

#include "DlgSettingsImp.h"
#include "ui_DlgSettings.h"


using namespace SpreadsheetGui;

/* TRANSLATOR SpreadsheetGui::DlgSettingsImp */

DlgSettingsImp::DlgSettingsImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettings)
{
    ui->setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsImp::~DlgSettingsImp() = default;

void DlgSettingsImp::saveSettings()
{

    /** use whatever the user has entered here
     *  we'll check for validity during import/export
     */
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Spreadsheet"
    );
    QString delimiter = ui->delimiterComboBox->currentText();
    hGrp->SetASCII("ImportExportDelimiter", delimiter.toStdString().c_str());
    ui->quoteCharLineEdit->onSave();
    ui->escapeCharLineEdit->onSave();
    ui->formatString->onSave();
    ui->dZLSpinBox->onSave();
    ui->checkBoxShowAlias->onSave();
}

void DlgSettingsImp::loadSettings()
{
    /** items "tab", ";", and "," have already been added to the combo box in the .ui file
     *  we'll recognize a few tokens: comma, semicolon, tab, and \t
     */

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Spreadsheet"
    );
    QString delimiter = QString::fromStdString(hGrp->GetASCII("ImportExportDelimiter", "tab"));
    int idx = ui->delimiterComboBox->findText(delimiter, Qt::MatchFixedString);
    if (idx != -1) {
        ui->delimiterComboBox->setCurrentIndex(idx);
    }
    else if (delimiter.compare(QLatin1String("\\t"), Qt::CaseInsensitive) == 0) {
        idx = ui->delimiterComboBox->findText(QLatin1String("tab"), Qt::MatchFixedString);
        ui->delimiterComboBox->setCurrentIndex(idx);
    }
    else if (delimiter.compare(QLatin1String("semicolon"), Qt::CaseInsensitive) == 0) {
        idx = ui->delimiterComboBox->findText(QLatin1String(";"), Qt::MatchFixedString);
        ui->delimiterComboBox->setCurrentIndex(idx);
    }
    else if (delimiter.compare(QLatin1String("comma"), Qt::CaseInsensitive) == 0) {
        idx = ui->delimiterComboBox->findText(QLatin1String(","), Qt::MatchFixedString);
        ui->delimiterComboBox->setCurrentIndex(idx);
    }
    else {
        ui->delimiterComboBox->addItem(delimiter);
        idx = ui->delimiterComboBox->findText(delimiter, Qt::MatchFixedString);
        ui->delimiterComboBox->setCurrentIndex(idx);
    }

    ui->quoteCharLineEdit->onRestore();
    ui->escapeCharLineEdit->onRestore();
    ui->formatString->onRestore();
    ui->dZLSpinBox->onRestore();
    ui->checkBoxShowAlias->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsImp.cpp"
