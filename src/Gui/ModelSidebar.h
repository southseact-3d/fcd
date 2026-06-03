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


#ifndef GUI_MODELSIDEBAR_H
#define GUI_MODELSIDEBAR_H

#include <QTreeView>
#include <QAbstractItemModel>
#include <vector>

#include <Base/Handled.h>
#include <Gui/Selection/Selection.h>

namespace App
{
class DocumentObject;
}

namespace Gui
{
class ViewProviderDocumentObject;

/** A tree model that shows Bodies and Sketches from the active document.
 *
 *  Layout:
 *    Bodies
 *      Body1
 *        Feature1
 *        Feature2
 *      Body2
 *    Sketches
 *      Sketch1
 *      Sketch2
 *
 *  When empty, shows a single "No bodies" placeholder row.
 */
class ModelSidebarModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    struct Item
    {
        QString name;
        QString typeName;
        QIcon icon;
        App::DocumentObject* object = nullptr;
        std::vector<std::unique_ptr<Item>> children;
        Item* parent = nullptr;
    };

    explicit ModelSidebarModel(QObject* parent = nullptr);
    ~ModelSidebarModel() override;

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /** Refresh the entire model from the current document. */
    void refresh();

    /** Returns the App::DocumentObject* for a given index, or nullptr. */
    App::DocumentObject* objectForIndex(const QModelIndex& index) const;

private Q_SLOTS:
    void onNewObject(const Gui::ViewProvider& vp);
    void onDeletedObject(const Gui::ViewProvider& vp);
    void onRelabelObject(const Gui::ViewProvider& vp);
    void onNewDocument(const Gui::Document& doc, bool isMain);
    void onDeletedDocument(const Gui::Document& doc);
    void onActiveDocument(const Gui::Document& doc);

private:
    void connectSignals();
    void disconnectSignals();
    bool isRelevantObject(App::DocumentObject* obj) const;
    bool isBody(App::DocumentObject* obj) const;
    bool isSketch(App::DocumentObject* obj) const;
    Item* itemFromIndex(const QModelIndex& index) const;

    std::unique_ptr<Item> _bodiesRoot;
    std::unique_ptr<Item> _sketchesRoot;
    std::vector<std::unique_ptr<Item>> _topLevel;
    bool _empty;
};


/** A tree view that displays Bodies and Sketches in a compact sidebar. */
class ModelSidebar: public QTreeView
{
    Q_OBJECT

public:
    explicit ModelSidebar(QWidget* parent = nullptr);
    ~ModelSidebar() override;

    /** Refresh the view from the current document. */
    void refresh();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private Q_SLOTS:
    void onDoubleClicked(const QModelIndex& index);

private:
    void setupStyle();

    ModelSidebarModel* _model;
};

}  // namespace Gui

#endif  // GUI_MODELSIDEBAR_H
