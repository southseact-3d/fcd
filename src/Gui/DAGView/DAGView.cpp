#include <QAbstractEventDispatcher>
#include <QVBoxLayout>
#include <memory>

#include <Gui/Application.h>
#include <Gui/Document.h>

#include "DAGView.h"
#include "DAGModel.h"


using namespace Gui;
using namespace DAG;
namespace sp = std::placeholders;

DAG::DockWindow::DockWindow(Gui::Document* gDocumentIn, QWidget* parent)
    : Gui::DockWindow(gDocumentIn, parent)
{
    dagView = new View(this);
    auto layout = new QVBoxLayout();
    layout->addWidget(dagView);
    this->setLayout(layout);
}

View::View(QWidget* parentIn)
    : QGraphicsView(parentIn)
{
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setRenderHint(QPainter::TextAntialiasing, true);
    // NOLINTBEGIN
    conActive = Application::Instance->signalActiveDocument.connect(
        std::bind(&View::slotActiveDocument, this, sp::_1)
    );
    conDelete = Application::Instance->signalDeleteDocument.connect(
        std::bind(&View::slotDeleteDocument, this, sp::_1)
    );
    // NOLINTEND

    // just update the dagview when the gui process is idle.
    connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, this, &View::awakeSlot);
}

View::~View() = default;

void View::slotActiveDocument(const Document& documentIn)
{
    if (Gui::Selection().hasSelection()) {
        return;
    }
    ModelMap::const_iterator it = modelMap.find(&documentIn);
    if (it == modelMap.end()) {
        ModelMap::value_type entry(
            std::make_pair(&documentIn, std::make_shared<Model>(this, documentIn))
        );
        modelMap.insert(entry);
        this->setScene(entry.second.get());
    }
    else {
        this->setScene(it->second.get());
    }
}

void View::slotDeleteDocument(const Document& documentIn)
{
    ModelMap::const_iterator it = modelMap.find(&documentIn);
    if (it != modelMap.end()) {
        modelMap.erase(it);
    }
}

void View::awakeSlot()
{
    Model* model = dynamic_cast<Model*>(this->scene());
    if (model) {
        model->awake();
    }
}

void View::onSelectionChanged(const SelectionChanges& msg)
{
    switch (msg.Type) {
        case SelectionChanges::AddSelection:
        case SelectionChanges::RmvSelection:
        case SelectionChanges::SetSelection:
            if (!msg.pDocName || !msg.pDocName[0]) {
                return;
            }
            break;
        case SelectionChanges::ClrSelection:
            if (!msg.pDocName || !msg.pDocName[0]) {
                for (auto& v : modelMap) {
                    v.second->selectionChanged(msg);
                }
                return;
            }
            break;
        default:
            return;
    }
    auto doc = Gui::Application::Instance->getDocument(msg.pDocName);
    if (!doc) {
        return;
    }
    auto& model = modelMap[doc];
    if (!model) {
        model = std::make_shared<Model>(this, *doc);
    }
    this->setScene(model.get());
    model->selectionChanged(msg);
}


#include "moc_DAGView.cpp"
