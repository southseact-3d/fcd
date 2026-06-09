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


#ifndef MESH_PARTTOOLSWIDGET_H
#define MESH_PARTTOOLSWIDGET_H

#include <QWidget>
#include <QVector>

class QToolButton;
class QMenu;

namespace MeshGui
{

class MeshGroupWidget;

class MeshPartToolsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshPartToolsWidget(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    MeshGroupWidget* createToolsGroup();
    MeshGroupWidget* createModifyGroup();
    MeshGroupWidget* createAnalyzeGroup();

    QToolButton* createToolButton(const char* cmdName);

    QMenu* buildToolsMenu();
    QMenu* buildModifyMenu();
    QMenu* buildAnalyzeMenu();

    static QIcon commandIcon(const char* cmdName);

    MeshGroupWidget* createGroup(
        const QString& label,
        const QVector<const char*>& visibleCommands,
        QMenu* menu);

    MeshGroupWidget* _toolsGroup = nullptr;
    MeshGroupWidget* _modifyGroup = nullptr;
    MeshGroupWidget* _analyzeGroup = nullptr;
};

}  // namespace MeshGui

#endif  // MESH_PARTTOOLSWIDGET_H
