// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 liamh <liamh[at]users.sourceforge.net>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "ModelSidebar.h"

#include <QHeaderView>
#include <QMouseEvent>

#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/MainWindow.h>

using namespace Gui;


// ===========================================================================
// ModelSidebarModel
// ===========================================================================

ModelSidebarModel::ModelSidebarModel(QObject* parent)
    : QAbstractItemModel(parent)
    , _empty(true)
{
    _bodiesRoot = std::make_unique<Item>();
    _bodiesRoot->name = tr("Bodies");
    _sketchesRoot = std::make_unique<Item>();
    _sketchesRoot->name = tr("Sketches");

    _topLevel.push_back(std::make_unique<Item>());
    _topLevel.back()->name = tr("No bodies");

    connectSignals();
    refresh();
}

ModelSidebarModel::~ModelSidebarModel() = default;

QModelIndex ModelSidebarModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column < 0 || column >= 1) {
        return {};
    }

    Item* parentItem = itemFromIndex(parent);
    if (!parentItem) {
        // Top-level
        if (row < 0 || row >= static_cast<int>(_topLevel.size())) {
            return {};
        }
        return createIndex(row, 0, _topLevel[row].get());
    }

    if (row < 0 || row >= static_cast<int>(parentItem->children.size())) {
        return {};
    }
    return createIndex(row, 0, parentItem->children[row].get());
}

QModelIndex ModelSidebarModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return {};
    }

    Item* item = itemFromIndex(index);
    if (!item || !item->parent) {
        return {};
    }

    Item* grandparent = item->parent->parent;
    if (!grandparent) {
        // parent is a top-level item
        for (int i = 0; i < static_cast<int>(_topLevel.size()); ++i) {
            if (_topLevel[i].get() == item->parent) {
                return createIndex(i, 0, item->parent);
            }
        }
        return {};
    }

    // Find row of parent within grandparent
    for (int i = 0; i < static_cast<int>(grandparent->children.size()); ++i) {
        if (grandparent->children[i].get() == item->parent) {
            return createIndex(i, 0, item->parent);
        }
    }
    return {};
}

int ModelSidebarModel::rowCount(const QModelIndex& parent) const
{
    Item* item = itemFromIndex(parent);
    if (!item) {
        return static_cast<int>(_topLevel.size());
    }
    return static_cast<int>(item->children.size());
}

int ModelSidebarModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ModelSidebarModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    Item* item = itemFromIndex(index);
    if (!item) {
        return {};
    }

    switch (role) {
        case Qt::DisplayRole:
            return item->name;
        case Qt::DecorationRole:
            return item->icon;
        case Qt::ToolTipRole:
            if (item->typeName.isEmpty()) {
                return item->name;
            }
            return QStringLiteral("%1 (%2)").arg(item->name, item->typeName);
        case Qt::UserRole:
            return QVariant::fromValue(reinterpret_cast<quintptr>(item->object));
        default:
            return {};
    }
}

Qt::ItemFlags ModelSidebarModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

App::DocumentObject* ModelSidebarModel::objectForIndex(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return nullptr;
    }
    Item* item = itemFromIndex(index);
    return item ? item->object : nullptr;
}

void ModelSidebarModel::refresh()
{
    beginResetModel();

    _bodiesRoot->children.clear();
    _sketchesRoot->children.clear();
    _topLevel.clear();

    auto* app = App::GetApplication::Instance();
    if (!app) {
        _empty = true;
        _topLevel.push_back(std::make_unique<Item>());
        _topLevel.back()->name = tr("No bodies");
        endResetModel();
        return;
    }

    auto* guiApp = Gui::Application::Instance;
    if (!guiApp) {
        _empty = true;
        _topLevel.push_back(std::make_unique<Item>());
        _topLevel.back()->name = tr("No bodies");
        endResetModel();
        return;
    }

    // Iterate all documents and their objects
    std::vector<App::Document*> docs = app->getDocuments();
    for (App::Document* doc : docs) {
        std::vector<App::DocumentObject*> objs = doc->getObjects();
        for (App::DocumentObject* obj : objs) {
            if (!obj) continue;

            auto* vpd = guiApp->getViewProvider(obj);
            if (!vpd) continue;

            if (isBody(obj)) {
                auto bodyItem = std::make_unique<Item>();
                bodyItem->name = QString::fromUtf8(obj->Label.getValue());
                bodyItem->typeName = QString::fromUtf8(obj->getTypeId().getName());
                bodyItem->icon = vpd->getIcon();
                bodyItem->object = obj;

                // Find features inside the body (via ViewProvider claimChildren)
                std::vector<App::DocumentObject*> children = obj->getInList();
                // We'll just show direct children for now
                std::vector<App::DocumentObject*> features;
                for (App::DocumentObject* child : children) {
                    if (child && child->getDocument() == doc) {
                        // Check if child is inside this body
                        auto* childVpd = guiApp->getViewProvider(child);
                        if (childVpd && !isBody(child) && !isSketch(child)) {
                            features.push_back(child);
                        }
                    }
                }

                _bodiesRoot->children.push_back(std::move(bodyItem));
            }
            else if (isSketch(obj)) {
                auto sketchItem = std::make_unique<Item>();
                sketchItem->name = QString::fromUtf8(obj->Label.getValue());
                sketchItem->typeName = QString::fromUtf8(obj->getTypeId().getName());
                sketchItem->icon = vpd->getIcon();
                sketchItem->object = obj;

                _sketchesRoot->children.push_back(std::move(sketchItem));
            }
        }
    }

    // Build top-level structure
    if (_bodiesRoot->children.empty() && _sketchesRoot->children.empty()) {
        _empty = true;
        _topLevel.push_back(std::make_unique<Item>());
        _topLevel.back()->name = tr("No bodies");
    }
    else {
        _empty = false;
        if (!_bodiesRoot->children.empty()) {
            auto bodiesCopy = std::make_unique<Item>();
            bodiesCopy->name = _bodiesRoot->name;
            for (auto& child : _bodiesRoot->children) {
                child->parent = bodiesCopy.get();
                bodiesCopy->children.push_back(std::move(child));
            }
            _topLevel.push_back(std::move(bodiesCopy));
        }
        if (!_sketchesRoot->children.empty()) {
            auto sketchesCopy = std::make_unique<Item>();
            sketchesCopy->name = _sketchesRoot->name;
            for (auto& child : _sketchesRoot->children) {
                child->parent = sketchesCopy.get();
                sketchesCopy->children.push_back(std::move(child));
            }
            _topLevel.push_back(std::move(sketchesCopy));
        }
    }

    endResetModel();
}

void ModelSidebarModel::onNewObject(const Gui::ViewProvider& vp)
{
    Q_UNUSED(vp);
    refresh();
}

void ModelSidebarModel::onDeletedObject(const Gui::ViewProvider& vp)
{
    Q_UNUSED(vp);
    refresh();
}

void ModelSidebarModel::onRelabelObject(const Gui::ViewProvider& vp)
{
    Q_UNUSED(vp);
    refresh();
}

void ModelSidebarModel::onNewDocument(const Gui::Document& doc, bool isMain)
{
    Q_UNUSED(doc);
    Q_UNUSED(isMain);
    refresh();
}

void ModelSidebarModel::onDeletedDocument(const Gui::Document& doc)
{
    Q_UNUSED(doc);
    refresh();
}

void ModelSidebarModel::onActiveDocument(const Gui::Document& doc)
{
    Q_UNUSED(doc);
    refresh();
}

void ModelSidebarModel::connectSignals()
{
    auto* guiApp = Gui::Application::Instance;
    if (!guiApp) return;

    guiApp->signalNewObject.connect([this](const Gui::ViewProvider& vp) {
        onNewObject(vp);
    });
    guiApp->signalDeletedObject.connect([this](const Gui::ViewProvider& vp) {
        onDeletedObject(vp);
    });
    guiApp->signalRelabelObject.connect([this](const Gui::ViewProvider& vp) {
        onRelabelObject(vp);
    });
    guiApp->signalNewDocument.connect([this](const Gui::Document& doc, bool isMain) {
        onNewDocument(doc, isMain);
    });
    guiApp->signalDeletedDocument.connect([this](const Gui::Document& doc) {
        onDeletedDocument(doc);
    });
    guiApp->signalActiveDocument.connect([this](const Gui::Document& doc) {
        onActiveDocument(doc);
    });
}

void ModelSidebarModel::disconnectSignals()
{
    // Signals are automatically disconnected when the Application is destroyed
}

bool ModelSidebarModel::isRelevantObject(App::DocumentObject* obj) const
{
    return isBody(obj) || isSketch(obj);
}

bool ModelSidebarModel::isBody(App::DocumentObject* obj) const
{
    if (!obj) return false;
    return obj->getTypeId().isDerivedFrom("PartDesign::Body")
        || obj->getTypeId().isDerivedFrom("Part::Body");
}

bool ModelSidebarModel::isSketch(App::DocumentObject* obj) const
{
    if (!obj) return false;
    return obj->getTypeId().isDerivedFrom("Sketcher::SketchObject");
}

ModelSidebarModel::Item* ModelSidebarModel::itemFromIndex(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return nullptr;
    }
    return static_cast<Item*>(index.internalPointer());
}


// ===========================================================================
// ModelSidebar
// ===========================================================================

ModelSidebar::ModelSidebar(QWidget* parent)
    : QTreeView(parent)
{
    _model = new ModelSidebarModel(this);
    setModel(_model);

    setHeaderHidden(true);
    setRootIsDecorated(true);
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setExpandsOnDoubleClick(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(false);

    // Compact styling
    setIndentation(16);
    setSpacing(2);

    header()->setStretchLastSection(true);
    header()->setVisible(false);

    setupStyle();

    connect(this, &QTreeView::doubleClicked, this, &ModelSidebar::onDoubleClicked);

    // Initial refresh
    _model->refresh();
}

ModelSidebar::~ModelSidebar() = default;

void ModelSidebar::refresh()
{
    _model->refresh();
    expandAll();
}

void ModelSidebar::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTreeView::mouseDoubleClickEvent(event);
}

void ModelSidebar::onDoubleClicked(const QModelIndex& index)
{
    App::DocumentObject* obj = _model->objectForIndex(index);
    if (!obj) return;

    // Try to edit the object (e.g. open sketch editor)
    try {
        Gui::Command::openCommand("Edit object");
        Gui::cmdSetEdit(obj, Gui::Application::Instance->getUserEditMode());
    }
    catch (const Base::Exception&) {
        Gui::Command::abortCommand();
    }
}

void ModelSidebar::setupStyle()
{
    setStyleSheet(QStringLiteral(
        "QTreeView {"
        "  background-color: transparent;"
        "  border: none;"
        "  font-size: 11px;"
        "  outline: none;"
        "}"
        "QTreeView::item {"
        "  padding: 3px 4px;"
        "  border: none;"
        "}"
        "QTreeView::item:selected {"
        "  background-color: rgba(0, 120, 212, 0.3);"
        "}"
        "QTreeView::item:hover {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "}"
        "QTreeView::branch {"
        "  background: transparent;"
        "}"
    ));
}

#include "moc_ModelSidebar.cpp"
