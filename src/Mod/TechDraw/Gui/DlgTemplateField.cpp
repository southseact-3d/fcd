#include "DlgTemplateField.h"
#include "ui_DlgTemplateField.h"


using namespace TechDrawGui;

DlgTemplateField::DlgTemplateField( QWidget *parent /* = nullptr */ ) :
    QDialog(parent), ui(new Ui_dlgTemplateField)
{
    ui->setupUi(this);
    ui->leInput->setFocus();
}

void DlgTemplateField::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgTemplateField::setFieldName(std::string name)
{
    QString qs = QString::fromUtf8(name.data(), name.size());
    ui->lblName->setText(qs);
}

void DlgTemplateField::setFieldLength(int length)
{
    ui->leInput->setMaxLength(length);
}

void DlgTemplateField::setFieldContent(std::string content)
{
    QString qs = QString::fromUtf8(content.data(), content.size());
    ui->leInput->setText(qs);
}

void DlgTemplateField::setAutofillContent(std::string content)
{
    QString qs = QString::fromUtf8(content.data(), content.size());
    ui->leAutofill->setText(qs);
}

QString DlgTemplateField::getFieldContent()
{
    return ui->leInput->text();
}

bool DlgTemplateField::getAutofillState()
{
    return ui->cbAutofill->isChecked();
}

void DlgTemplateField::accept()
{
    QDialog::accept();
}

void DlgTemplateField::reject()
{
    QDialog::reject();
}

#include <Mod/TechDraw/Gui/moc_DlgTemplateField.cpp>
