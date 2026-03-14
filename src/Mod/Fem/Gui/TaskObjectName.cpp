#include <QString>


#include <App/DocumentObject.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Document.h>

#include "TaskObjectName.h"
#include "ui_TaskObjectName.h"


using namespace FemGui;
using namespace Gui;

TaskObjectName::TaskObjectName(App::DocumentObject* pcObject, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("FEM_CreateNodesSet"), tr("Name of the object"), true, parent)
    , pcObject(pcObject)
    , ui(new Ui_TaskObjectName)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    QObject::connect(ui->lineEdit_ObjectName, &QLineEdit::textChanged, this, &TaskObjectName::TextChanged);

    if (strcmp(pcObject->Label.getValue(), "") != 0) {
        ui->lineEdit_ObjectName->setText(QString::fromUtf8(pcObject->Label.getValue()));
    }
    else {
        ui->lineEdit_ObjectName->setText(QString::fromLatin1(pcObject->getNameInDocument()));
    }
}

void TaskObjectName::TextChanged(const QString& text)
{
    name = text.toUtf8().constData();
    // pcObject->Label.setValue(text.toUtf8());
}

TaskObjectName::~TaskObjectName() = default;


#include "moc_TaskObjectName.cpp"
