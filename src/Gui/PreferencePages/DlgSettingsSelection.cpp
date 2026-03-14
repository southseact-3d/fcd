#include <App/Application.h>

#include "DlgSettingsSelection.h"
#include "ui_DlgSettingsSelection.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgSettingsSelection */

DlgSettingsSelection::DlgSettingsSelection(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsSelection)
{
    ui->setupUi(this);
}

DlgSettingsSelection::~DlgSettingsSelection() = default;

void DlgSettingsSelection::saveSettings()
{
    ui->checkBoxPreselection->onSave();
    ui->checkBoxSelection->onSave();
    ui->HighlightColor->onSave();
    ui->SelectionColor->onSave();
    ui->spinPickRadius->onSave();
    ui->checkBoxAutoSwitch->onSave();
    ui->checkBoxAutoExpand->onSave();
    ui->checkBoxPreselect->onSave();
    ui->checkBoxRecord->onSave();
    ui->checkBoxSelectionCheckBoxes->onSave();
}

void DlgSettingsSelection::loadSettings()
{
    ui->checkBoxPreselection->onRestore();
    ui->checkBoxSelection->onRestore();
    ui->HighlightColor->onRestore();
    ui->SelectionColor->onRestore();
    ui->spinPickRadius->onRestore();
    ui->checkBoxAutoSwitch->onRestore();
    ui->checkBoxAutoExpand->onRestore();
    ui->checkBoxPreselect->onRestore();
    ui->checkBoxRecord->onRestore();
    ui->checkBoxSelectionCheckBoxes->onRestore();
}

void DlgSettingsSelection::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsSelection.cpp"
