#include <QMenu>
#include <QMessageBox>


#include <Gui/MainWindow.h>

#include <Mod/Material/App/Exceptions.h>
#include <Mod/Material/App/Materials.h>

#include "TextEdit.h"
#include "ui_TextEdit.h"


using namespace MatGui;

/* TRANSLATOR MatGui::TextEdit */

TextEdit::TextEdit(const QString& propertyName,
                   const std::shared_ptr<Materials::Material>& material,
                   QWidget* parent)
    : QDialog(parent)
    , ui(new Ui_TextEdit)
    , _material(material)
{
    ui->setupUi(this);

    if (material->hasPhysicalProperty(propertyName)) {
        _property = material->getPhysicalProperty(propertyName);
    }
    else if (material->hasAppearanceProperty(propertyName)) {
        _property = material->getAppearanceProperty(propertyName);
    }
    else {
        Base::Console().log("Property '%s' not found\n", propertyName.toStdString().c_str());
        _property = nullptr;
    }
    if (_property) {
        _value = _property->getString();
    }
    else {
        Base::Console().log("No value loaded\n");
        _value = QString();
    }

    ui->textEdit->setText(_value);
    ui->textEdit->setAcceptRichText(false);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);

    connect(ui->standardButtons, &QDialogButtonBox::accepted, this, &TextEdit::accept);
    connect(ui->standardButtons, &QDialogButtonBox::rejected, this, &TextEdit::reject);
}

void TextEdit::accept()
{
    QString newText = ui->textEdit->toPlainText();
    if (newText != _value) {
        _property->setValue(ui->textEdit->toPlainText());
        _material->setEditStateAlter();
    }
    QDialog::accept();
}

void TextEdit::reject()
{
    QDialog::reject();
}

#include "moc_TextEdit.cpp"
