#include "DlgSettingsPythonConsole.h"
#include "ui_DlgSettingsPythonConsole.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgSettingsPythonConsole */

DlgSettingsPythonConsole::DlgSettingsPythonConsole(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsPythonConsole)
{
    ui->setupUi(this);
}

DlgSettingsPythonConsole::~DlgSettingsPythonConsole() = default;

void DlgSettingsPythonConsole::saveSettings()
{
    ui->PythonWordWrap->onSave();
    ui->PythonBlockCursor->onSave();
    ui->PythonSaveHistory->onSave();
    ui->ProfilerInterval->onSave();
    ui->PathToPythonExecutable->onSave();
}

void DlgSettingsPythonConsole::loadSettings()
{
    ui->PythonWordWrap->onRestore();
    ui->PythonBlockCursor->onRestore();
    ui->PythonSaveHistory->onRestore();
    ui->ProfilerInterval->onRestore();
    ui->PathToPythonExecutable->onRestore();
}

void DlgSettingsPythonConsole::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

#include "moc_DlgSettingsPythonConsole.cpp"
