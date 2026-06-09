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


#include "PointsPartToolsWidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Action.h>
#include <Gui/BitmapFactory.h>

using namespace PointsGui;

namespace
{

const int MinButtonWidth = 36;
const int IconSize = 20;

const QString labelSheet = QStringLiteral(
    "QToolButton {"
    "  border: none;"
    "  border-top: 1px solid @LabelBorderTopColor;"
    "  padding: 2px 4px;"
    "  font-size: @LabelFontSize;"
    "  color: @LabelTextColor;"
    "  text-transform: uppercase;"
    "  letter-spacing: 0.5px;"
    "}"
    "QToolButton:hover { color: @LabelHoverColor; }"
);

const QString groupToolSheet = QStringLiteral(
    "QToolButton {"
    "  border: 1px solid transparent;"
    "  border-radius: @ToolButtonBorderRadius;"
    "  padding: 2px;"
    "}"
    "QToolButton:hover {"
    "  background: @ToolButtonHoverBackground;"
    "  border-color: @ToolButtonHoverBorderColor;"
    "}"
    "QToolButton:pressed {"
    "  background: @ToolButtonPressedBackground;"
    "}"
);

}  // namespace


// ============================================================================
// PointsGroupWidget — a labeled tool group (tools on top, label below)
// ============================================================================

class PointsGui::PointsGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointsGroupWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        _toolsContainer = new QWidget(this);
        _toolsLayout = new QHBoxLayout(_toolsContainer);
        _toolsLayout->setContentsMargins(2, 2, 2, 0);
        _toolsLayout->setSpacing(1);
        _toolsLayout->addStretch();

        _labelButton = new QToolButton(this);
        _labelButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        _labelButton->setAutoRaise(true);
        _labelButton->setStyleSheet(
            Gui::Application::Instance->replaceVariablesInQss(labelSheet));
        _labelButton->setCursor(Qt::PointingHandCursor);
        _labelButton->setFixedHeight(20);

        mainLayout->addWidget(_toolsContainer, 1);
        mainLayout->addWidget(_labelButton, 0);
    }

    QToolButton* addButton(QToolButton* btn)
    {
        btn->setParent(_toolsContainer);
        btn->setIconSize(QSize(IconSize, IconSize));
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setAutoRaise(true);
        btn->setStyleSheet(
            Gui::Application::Instance->replaceVariablesInQss(groupToolSheet));
        btn->setMinimumWidth(MinButtonWidth);
        btn->setMaximumWidth(MinButtonWidth + 8);
        btn->setFixedHeight(IconSize + 8);

        _toolsButtons.append(btn);

        int count = _toolsLayout->count();
        _toolsLayout->insertWidget(count - 1, btn);

        return btn;
    }

    QToolButton* labelButton() const { return _labelButton; }

    void setLabelText(const QString& text)
    {
        _labelButton->setText(text + QStringLiteral("  \u25BC"));
    }

    void setMenu(QMenu* menu)
    {
        _menu = menu;
        connect(_labelButton, &QToolButton::clicked, this, &PointsGroupWidget::showMenu);
    }

    const QVector<QToolButton*>& toolButtons() const { return _toolsButtons; }

    void updateVisibility(int availableWidth)
    {
        int maxVisible = availableWidth / MinButtonWidth;
        if (maxVisible < 1) {
            maxVisible = 1;
        }

        int visible = 0;
        for (int i = 0; i < _toolsButtons.size(); ++i) {
            if (visible < maxVisible) {
                _toolsButtons[i]->show();
                visible++;
            }
            else {
                _toolsButtons[i]->hide();
            }
        }
    }

private Q_SLOTS:
    void showMenu()
    {
        if (_menu) {
            QPoint pos = _labelButton->mapToGlobal(QPoint(0, _labelButton->height()));
            _menu->exec(pos);
        }
    }

private:
    QWidget* _toolsContainer = nullptr;
    QHBoxLayout* _toolsLayout = nullptr;
    QToolButton* _labelButton = nullptr;
    QMenu* _menu = nullptr;
    QVector<QToolButton*> _toolsButtons;
};


// ============================================================================
// PointsPartToolsWidget
// ============================================================================

PointsPartToolsWidget::PointsPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(4);

    _ioGroup = createIOGroup();
    _modifyGroup = createModifyGroup();

    layout->addWidget(_ioGroup, 1);
    layout->addWidget(_modifyGroup, 1);
}

void PointsPartToolsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int totalWidth = event->size().width();
    int groupWidth = totalWidth / 2;

    if (_ioGroup) {
        _ioGroup->updateVisibility(groupWidth);
    }
    if (_modifyGroup) {
        _modifyGroup->updateVisibility(groupWidth);
    }
}

PointsGroupWidget* PointsPartToolsWidget::createIOGroup()
{
    QVector<const char*> commands = {
        "Points_Import",
        "Points_Export",
    };

    auto* group = createGroup(
        QStringLiteral("IO"),
        commands,
        buildIOMenu());

    return group;
}

PointsGroupWidget* PointsPartToolsWidget::createModifyGroup()
{
    QVector<const char*> commands = {
        "Points_Convert",
        "Points_Structure",
        "Points_Merge",
        "Points_PolyCut",
    };

    auto* group = createGroup(
        QStringLiteral("Modify"),
        commands,
        buildModifyMenu());

    return group;
}

PointsGroupWidget* PointsPartToolsWidget::createGroup(
    const QString& label,
    const QVector<const char*>& visibleCommands,
    QMenu* menu)
{
    auto* group = new PointsGroupWidget(this);
    group->setLabelText(label);
    group->setMenu(menu);

    for (const char* cmd : visibleCommands) {
        auto* btn = createToolButton(cmd);
        if (btn) {
            group->addButton(btn);
        }
    }

    return group;
}

QToolButton* PointsPartToolsWidget::createToolButton(const char* cmdName)
{
    auto& cmdMgr = Gui::Application::Instance->commandManager();
    Gui::Command* cmd = cmdMgr.getCommandByName(cmdName);
    if (!cmd) {
        return nullptr;
    }

    Gui::Action* action = cmd->getAction();
    if (!action) {
        return nullptr;
    }

    auto* btn = new QToolButton(this);
    btn->setDefaultAction(action->action());
    btn->setIcon(commandIcon(cmdName));
    btn->setToolTip(
        QApplication::translate(cmd->className(), cmd->getToolTipText()));
    btn->setText(
        QApplication::translate(cmd->className(), cmd->getMenuText()));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setIconSize(QSize(IconSize, IconSize));
    btn->setAutoRaise(true);
    btn->setStyleSheet(
        Gui::Application::Instance->replaceVariablesInQss(groupToolSheet));

    return btn;
}

QMenu* PointsPartToolsWidget::buildIOMenu()
{
    auto* menu = new QMenu(this);

    auto& cmdMgr = Gui::Application::Instance->commandManager();

    auto addMenuItem = [menu, &cmdMgr](const char* cmdName) {
        Gui::Command* cmd = cmdMgr.getCommandByName(cmdName);
        if (cmd) {
            Gui::Action* action = cmd->getAction();
            if (action) {
                menu->addAction(action->action());
            }
        }
    };

    auto addSeparator = [menu]() {
        menu->addSeparator();
    };

    auto addSectionHeader = [menu](const QString& text) {
        auto* action = new QAction(text, menu);
        action->setEnabled(false);
        QFont font = action->font();
        font.setPointSize(9);
        font.setWeight(QFont::DemiBold);
        action->setFont(font);
        menu->addAction(action);
    };

    // Import/Export section
    addSectionHeader(QObject::tr("IMPORT/EXPORT"));
    addMenuItem("Points_Import");
    addMenuItem("Points_Export");

    return menu;
}

QMenu* PointsPartToolsWidget::buildModifyMenu()
{
    auto* menu = new QMenu(this);

    auto& cmdMgr = Gui::Application::Instance->commandManager();

    auto addMenuItem = [menu, &cmdMgr](const char* cmdName) {
        Gui::Command* cmd = cmdMgr.getCommandByName(cmdName);
        if (cmd) {
            Gui::Action* action = cmd->getAction();
            if (action) {
                menu->addAction(action->action());
            }
        }
    };

    auto addSeparator = [menu]() {
        menu->addSeparator();
    };

    auto addSectionHeader = [menu](const QString& text) {
        auto* action = new QAction(text, menu);
        action->setEnabled(false);
        QFont font = action->font();
        font.setPointSize(9);
        font.setWeight(QFont::DemiBold);
        action->setFont(font);
        menu->addAction(action);
    };

    // Convert section
    addSectionHeader(QObject::tr("CONVERT"));
    addMenuItem("Points_Convert");

    addSeparator();

    // Structure section
    addSectionHeader(QObject::tr("STRUCTURE"));
    addMenuItem("Points_Structure");

    addSeparator();

    // Editing section
    addSectionHeader(QObject::tr("EDITING"));
    addMenuItem("Points_Merge");
    addMenuItem("Points_PolyCut");

    return menu;
}

QIcon PointsPartToolsWidget::commandIcon(const char* cmdName)
{
    auto& cmdMgr = Gui::Application::Instance->commandManager();
    Gui::Command* cmd = cmdMgr.getCommandByName(cmdName);
    if (!cmd) {
        return {};
    }
    Gui::Action* action = cmd->getAction();
    if (action) {
        return action->icon();
    }
    return {};
}

#include "PointsPartToolsWidget.moc"
