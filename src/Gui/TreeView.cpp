#include <QMouseEvent>


#include "TreeView.h"
#include "Application.h"
#include "Document.h"
#include "DocumentModel.h"
#include "MainWindow.h"
#include "MDIView.h"
#include "ViewProvider.h"

using namespace Gui;

TreeView::TreeView(QWidget* parent)
    : QTreeView(parent)
{
    setModel(new DocumentModel(this));
    QModelIndex root = this->model()->index(0, 0, QModelIndex());
    this->setExpanded(root, true);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(false);
    this->setRootIsDecorated(false);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setMouseTracking(true);  // needed for itemEntered() to work
}

TreeView::~TreeView() = default;

void TreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid() || index.internalPointer() == Application::Instance) {
        return;
    }
    Base::BaseClass* item = nullptr;
    item = static_cast<Base::BaseClass*>(index.internalPointer());
    if (item->is<Document>()) {
        QTreeView::mouseDoubleClickEvent(event);
        const Gui::Document* doc = static_cast<Gui::Document*>(item);
        MDIView* view = doc->getActiveView();
        if (!view) {
            return;
        }
        getMainWindow()->setActiveWindow(view);
    }
    else if (item->isDerivedFrom<ViewProvider>()) {
        if (!static_cast<ViewProvider*>(item)->doubleClicked()) {
            QTreeView::mouseDoubleClickEvent(event);
        }
    }
}

void TreeView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);
    if (parent.isValid()) {
        auto ptr = static_cast<Base::BaseClass*>(parent.internalPointer());
        // type is defined in DocumentModel.cpp
        if (ptr->getTypeId() == Base::Type::fromName("Gui::ApplicationIndex")) {
            for (int i = start; i <= end; i++) {
                QModelIndex document = this->model()->index(i, 0, parent);
                this->expand(document);
            }
        }
    }
}

#include "moc_TreeView.cpp"
