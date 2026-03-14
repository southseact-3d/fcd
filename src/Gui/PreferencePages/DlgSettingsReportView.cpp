#include "DlgSettingsReportView.h"
#include "ui_DlgSettingsReportView.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgSettingsReportView */

/**
 *  Constructs a DlgSettingsReportView which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgSettingsReportView::DlgSettingsReportView(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsReportView)
{
    ui->setupUi(this);
    ui->colorText->setColor(qApp->palette().windowText().color());
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsReportView::~DlgSettingsReportView() = default;

void DlgSettingsReportView::saveSettings()
{
    ui->checkMessage->onSave();
    ui->checkLogging->onSave();
    ui->checkWarning->onSave();
    ui->checkError->onSave();
    ui->checkShowReportViewOnWarning->onSave();
    ui->checkShowReportViewOnError->onSave();
    ui->checkShowReportViewOnNormalMessage->onSave();
    ui->checkShowReportViewOnLogMessage->onSave();
    ui->checkShowReportTimecode->onSave();
    ui->colorText->onSave();
    ui->colorLogging->onSave();
    ui->colorWarning->onSave();
    ui->colorError->onSave();
    ui->pythonOutput->onSave();
    ui->pythonError->onSave();
}

void DlgSettingsReportView::loadSettings()
{
    ui->checkMessage->onRestore();
    ui->checkLogging->onRestore();
    ui->checkWarning->onRestore();
    ui->checkError->onRestore();
    ui->checkShowReportViewOnWarning->onRestore();
    ui->checkShowReportViewOnError->onRestore();
    ui->checkShowReportViewOnNormalMessage->onRestore();
    ui->checkShowReportViewOnLogMessage->onRestore();
    ui->checkShowReportTimecode->onRestore();
    ui->colorText->onRestore();
    ui->colorLogging->onRestore();
    ui->colorWarning->onRestore();
    ui->colorError->onRestore();
    ui->pythonOutput->blockSignals(true);
    ui->pythonOutput->onRestore();
    ui->pythonOutput->blockSignals(false);
    ui->pythonError->blockSignals(true);
    ui->pythonError->onRestore();
    ui->pythonError->blockSignals(false);
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsReportView::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsReportView.cpp"
