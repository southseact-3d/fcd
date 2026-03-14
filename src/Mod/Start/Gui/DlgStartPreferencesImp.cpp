#include <Gui/Application.h>

#include "DlgStartPreferencesImp.h"
#include "ui_DlgStartPreferences.h"


using namespace StartGui;

/**
 *  Constructs a DlgStartPreferencesImp which is a child of 'parent'
 */
DlgStartPreferencesImp::DlgStartPreferencesImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgStartPreferences)
{
    ui->setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgStartPreferencesImp::~DlgStartPreferencesImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgStartPreferencesImp::saveSettings()
{
    ui->fileChooserCustomFolder->onSave();
    ui->checkBoxShowExamples->onSave();
    ui->checkBoxCloseAfterLoading->onSave();
    ui->checkBoxShowOnlyFCStd->onSave();
}

void DlgStartPreferencesImp::loadSettings()
{
    ui->fileChooserCustomFolder->onRestore();
    ui->checkBoxShowExamples->onRestore();
    ui->checkBoxCloseAfterLoading->onRestore();
    ui->checkBoxShowOnlyFCStd->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgStartPreferencesImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgStartPreferencesImp.cpp"
