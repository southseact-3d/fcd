#include <QMessageBox>


#include <App/Application.h>
#include <Gui/Application.h>

#include "ui_DlgActiveBody.h"
#include "DlgActiveBody.h"
#include "ReferenceSelection.h"
#include "Utils.h"

Q_DECLARE_METATYPE(App::DocumentObject*)

using namespace PartDesignGui;

DlgActiveBody::DlgActiveBody(QWidget* parent, App::Document*& doc, const QString& infoText)
    : QDialog(parent)
    , ui(new Ui_DlgActiveBody)
    , _doc(doc)
    , activeBody(nullptr)
{
    ui->setupUi(this);

    connect(ui->bodySelect, &QListWidget::itemDoubleClicked, this, &DlgActiveBody::accept);

    if (!infoText.isEmpty()) {
        ui->label->setText(infoText + QStringLiteral("\n\n") + tr("Please select"));
    }

    auto bodies = _doc->getObjectsOfType(PartDesign::Body::getClassTypeId());

    PartDesign::Body* bodyOfActiveObject = nullptr;
    for (const auto& obj : Gui::Selection().getSelection()) {
        bodyOfActiveObject = PartDesign::Body::findBodyOf(obj.pObject);
        break;  // Just get the body for first selected object
    }

    for (const auto& body : bodies) {
        auto item = new QListWidgetItem(QString::fromUtf8(body->Label.getValue()));
        item->setData(Qt::UserRole, QVariant::fromValue(body));
        ui->bodySelect->addItem(item);

        if (body == bodyOfActiveObject) {
            item->setSelected(true);
        }

        // TODO: Any other logic (hover, select effects on view etc.)
    }

    if (!bodyOfActiveObject) {
        // by default select the first item so that the user
        // can continue by clicking Ok without further action
        QListWidgetItem* first = ui->bodySelect->item(0);
        if (first) {
            first->setSelected(true);
        }
    }
}

DlgActiveBody::~DlgActiveBody() = default;

void DlgActiveBody::accept()
{
    auto selectedItems = ui->bodySelect->selectedItems();
    if (selectedItems.empty()) {
        return;
    }

    App::DocumentObject* selectedBody
        = selectedItems[0]->data(Qt::UserRole).value<App::DocumentObject*>();
    if (selectedBody) {
        activeBody = makeBodyActive(selectedBody, _doc);
    }
    else {
        // A transaction must be created as otherwise the undo/redo is broken
        App::GetApplication().setActiveTransaction(QT_TRANSLATE_NOOP("Command", "Add a Body"), true);
        activeBody = makeBody(_doc);
        App::GetApplication().closeActiveTransaction();
    }

    QDialog::accept();
}

#include "moc_DlgActiveBody.cpp"
