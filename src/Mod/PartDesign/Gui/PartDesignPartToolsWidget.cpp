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


#include "PartDesignPartToolsWidget.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QToolButton>

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Action.h>
#include <Gui/BitmapFactory.h>

using namespace PartDesignGui;

namespace
{

const QString buttonStyleSheet = QStringLiteral(
    "QToolButton { white-space: pre-wrap; text-align: center; }");

QIcon commandIcon(const char* cmdName)
{
    auto* cmdMgr = Gui::Application::Instance->commandManager();
    Gui::Command* cmd = cmdMgr->getCommand(cmdName);
    if (!cmd) {
        return {};
    }
    Gui::Action* action = cmd->getAction();
    if (action) {
        return action->icon();
    }
    return {};
}

}  // namespace

PartDesignPartToolsWidget::PartDesignPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(2);

    // Group 1: Primitives (equal stretch)
    layout->addWidget(createPrimitiveGroup(), 1);

    // Group 2: Create dropdown (equal stretch)
    layout->addWidget(createCreateGroup(), 1);

    // Group 3: Modify + Inspect dropdowns (equal stretch)
    layout->addWidget(createModifyInspectGroup(), 1);
}

QWidget* PartDesignPartToolsWidget::createPrimitiveGroup()
{
    auto* group = new QWidget(this);
    auto* layout = new QHBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    const char* primitives[] = {
        "PartDesign_PartBox",
        "PartDesign_PartCylinder",
        "PartDesign_PartSphere",
        "PartDesign_PartCone",
        "PartDesign_PartTorus",
    };

    for (const char* cmd : primitives) {
        layout->addWidget(createPrimitiveButton(QString::fromLatin1(cmd)));
    }

    return group;
}

QWidget* PartDesignPartToolsWidget::createCreateGroup()
{
    auto* group = new QWidget(this);
    auto* layout = new QHBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createDropdownButton(QStringLiteral("PartDesign_CompCreate")));

    return group;
}

QWidget* PartDesignPartToolsWidget::createModifyInspectGroup()
{
    auto* group = new QWidget(this);
    auto* layout = new QHBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(createDropdownButton(QStringLiteral("PartDesign_CompModify")));
    layout->addWidget(createDropdownButton(QStringLiteral("PartDesign_CompInspect")));

    return group;
}

QToolButton* PartDesignPartToolsWidget::createPrimitiveButton(const QString& cmdName)
{
    auto* cmdMgr = Gui::Application::Instance->commandManager();
    Gui::Command* cmd = cmdMgr->getCommand(cmdName.toLatin1().constData());

    auto* btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(20, 20));
    btn->setStyleSheet(buttonStyleSheet);
    btn->setAutoRaise(true);

    if (cmd) {
        Gui::Action* action = cmd->getAction();
        if (action) {
            btn->setDefaultAction(action->action());
        }
        btn->setIcon(commandIcon(cmdName.toLatin1().constData()));
        btn->setToolTip(
            QApplication::translate(cmd->className(), cmd->getToolTipText()));
        btn->setText(
            QApplication::translate(cmd->className(), cmd->getMenuText()));
    }

    return btn;
}

QToolButton* PartDesignPartToolsWidget::createDropdownButton(const QString& groupCmdName)
{
    auto* cmdMgr = Gui::Application::Instance->commandManager();
    Gui::Command* cmd = cmdMgr->getCommand(groupCmdName.toLatin1().constData());

    auto* btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(20, 20));
    btn->setStyleSheet(buttonStyleSheet);
    btn->setAutoRaise(true);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    if (cmd) {
        Gui::ActionGroup* groupAction =
            qobject_cast<Gui::ActionGroup*>(cmd->getAction());
        if (groupAction) {
            btn->setDefaultAction(groupAction->action());

            auto* menu = new QMenu(btn);
            menu->addActions(groupAction->groupAction()->actions());
            btn->setMenu(menu);

            btn->setIcon(groupAction->action()->icon());
            btn->setToolTip(
                QApplication::translate(cmd->className(), cmd->getToolTipText()));
            btn->setText(
                QApplication::translate(cmd->className(), cmd->getMenuText()));
        }
    }

    return btn;
}
