#include <QDialog>


#include <Gui/MainWindow.h>

#include "DlgProcessorChooser.h"
#include "ui_DlgProcessorChooser.h"


using namespace PathGui;

/* TRANSLATOR PathGui::DlgProcessorChooser */

DlgProcessorChooser::DlgProcessorChooser(std::vector<std::string>& scriptnames, bool withArguments)
    : QDialog(Gui::getMainWindow())
    , ui(new Ui_DlgProcessorChooser)
{
    ui->setupUi(this);
    ui->comboBox->addItem(tr("None"));
    for (std::vector<std::string>::const_iterator it = scriptnames.begin(); it != scriptnames.end();
         ++it) {
        ui->comboBox->addItem(QString::fromUtf8((*it).c_str()));
    }
    QMetaObject::connectSlotsByName(this);
    if (withArguments) {
        ui->argsLabel->setEnabled(true);
        ui->argsLineEdit->setEnabled(true);
    }
}

DlgProcessorChooser::~DlgProcessorChooser()
{
    delete ui;
}

std::string DlgProcessorChooser::getProcessor()
{
    return processor;
}

std::string DlgProcessorChooser::getArguments()
{
    return arguments;
}

void DlgProcessorChooser::accept()
{
    if (ui->comboBox->currentText() == tr("None")) {
        processor = "";
        arguments = "";
    }
    else {
        processor = ui->comboBox->currentText().toUtf8().data();
        arguments = ui->argsLineEdit->text().toUtf8().data();
    }
    QDialog::accept();
}
#include "moc_DlgProcessorChooser.cpp"
