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

using namespace PartDesignGui;

namespace
{

const int MinButtonWidth = 36;
const int IconSize = 20;

const QString labelSheet = QStringLiteral(
    "QToolButton {"
    "  border: none;"
    "  border-top: 1px solid #ccc;"
    "  padding: 2px 4px;"
    "  font-size: 10px;"
    "  color: #555;"
    "  text-transform: uppercase;"
    "  letter-spacing: 0.5px;"
    "}"
    "QToolButton:hover { color: #0078d4; }"
);

const QString groupToolSheet = QStringLiteral(
    "QToolButton {"
    "  border: 1px solid transparent;"
    "  border-radius: 3px;"
    "  padding: 2px;"
    "}"
    "QToolButton:hover {"
    "  background: #d0d0d0;"
    "  border-color: #aaa;"
    "}"
    "QToolButton:pressed {"
    "  background: #b8b8b8;"
    "}"
);

}  // namespace


// ============================================================================
// PartDesignGroupWidget — a labeled tool group (tools on top, label below)
// ============================================================================

class PartDesignGui::PartDesignGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PartDesignGroupWidget(QWidget* parent = nullptr)
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
        _labelButton->setStyleSheet(labelSheet);
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
        btn->setStyleSheet(groupToolSheet);
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
        connect(_labelButton, &QToolButton::clicked, this, &PartDesignGroupWidget::showMenu);
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
// PartDesignPartToolsWidget
// ============================================================================

PartDesignPartToolsWidget::PartDesignPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(4);

    _createGroup = createCreateGroup();
    _modifyGroup = createModifyGroup();
    _inspectGroup = createInspectGroup();

    layout->addWidget(_createGroup, 1);
    layout->addWidget(_modifyGroup, 1);
    layout->addWidget(_inspectGroup, 1);
}

void PartDesignPartToolsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int totalWidth = event->size().width();
    int groupWidth = totalWidth / 3;

    if (_createGroup) {
        _createGroup->updateVisibility(groupWidth);
    }
    if (_modifyGroup) {
        _modifyGroup->updateVisibility(groupWidth);
    }
    if (_inspectGroup) {
        _inspectGroup->updateVisibility(groupWidth);
    }
}

PartDesignGroupWidget* PartDesignPartToolsWidget::createCreateGroup()
{
    QVector<const char*> commands = {
        "PartDesign_NewSketch",
        "PartDesign_Pad",
        "PartDesign_Revolution",
        "PartDesign_AdditiveLoft",
        "PartDesign_Pocket",
        "PartDesign_Hole",
        "PartDesign_Groove",
        "PartDesign_PartBox",
        "PartDesign_PartCylinder",
        "PartDesign_Boolean",
    };

    auto* group = createGroup(
        QStringLiteral("Create"),
        commands,
        buildCreateMenu());

    return group;
}

PartDesignGroupWidget* PartDesignPartToolsWidget::createModifyGroup()
{
    QVector<const char*> commands = {
        "Std_TransformManip",
        "PartDesign_PartScale",
        "PartDesign_Fillet",
        "PartDesign_Chamfer",
        "PartDesign_Draft",
        "PartDesign_Thickness",
        "PartDesign_Mirrored",
        "PartDesign_LinearPattern",
        "PartDesign_PolarPattern",
        "PartDesign_MultiTransform",
    };

    auto* group = createGroup(
        QStringLiteral("Modify"),
        commands,
        buildModifyMenu());

    return group;
}

PartDesignGroupWidget* PartDesignPartToolsWidget::createInspectGroup()
{
    QVector<const char*> commands = {
        "PartDesign_PartCheckGeometry",
        "Std_Measure",
    };

    auto* group = createGroup(
        QStringLiteral("Inspect"),
        commands,
        buildInspectMenu());

    return group;
}

PartDesignGroupWidget* PartDesignPartToolsWidget::createGroup(
    const QString& label,
    const QVector<const char*>& visibleCommands,
    QMenu* menu)
{
    auto* group = new PartDesignGroupWidget(this);
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

QToolButton* PartDesignPartToolsWidget::createToolButton(const char* cmdName)
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
    btn->setStyleSheet(groupToolSheet);

    return btn;
}

QMenu* PartDesignPartToolsWidget::buildCreateMenu()
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

    // Sketch section
    addSectionHeader(QObject::tr("SKETCH"));
    addMenuItem("PartDesign_NewSketch");

    addSeparator();

    // Additive section
    addSectionHeader(QObject::tr("ADDITIVE"));
    addMenuItem("PartDesign_Pad");
    addMenuItem("PartDesign_Revolution");
    addMenuItem("PartDesign_AdditiveLoft");
    addMenuItem("PartDesign_AdditivePipe");
    addMenuItem("PartDesign_AdditiveHelix");
    addMenuItem("PartDesign_CompPrimitiveAdditive");

    addSeparator();

    // Subtractive section
    addSectionHeader(QObject::tr("SUBTRACTIVE"));
    addMenuItem("PartDesign_Pocket");
    addMenuItem("PartDesign_Hole");
    addMenuItem("PartDesign_Groove");
    addMenuItem("PartDesign_SubtractiveLoft");
    addMenuItem("PartDesign_SubtractivePipe");
    addMenuItem("PartDesign_SubtractiveHelix");
    addMenuItem("PartDesign_CompPrimitiveSubtractive");

    addSeparator();

    // Primitives section
    addSectionHeader(QObject::tr("PRIMITIVES"));
    addMenuItem("PartDesign_PartBox");
    addMenuItem("PartDesign_PartCylinder");
    addMenuItem("PartDesign_PartSphere");
    addMenuItem("PartDesign_PartCone");
    addMenuItem("PartDesign_PartTorus");
    addMenuItem("PartDesign_PartTube");
    addMenuItem("PartDesign_PartPrimitives");

    addSeparator();

    // Boolean section
    addSectionHeader(QObject::tr("BOOLEAN"));
    addMenuItem("PartDesign_Boolean");
    addMenuItem("PartDesign_PartBuilder");
    addMenuItem("PartDesign_PartCut");
    addMenuItem("PartDesign_PartFuse");
    addMenuItem("PartDesign_PartCommon");
    addMenuItem("PartDesign_PartSection");

    addSeparator();

    // Join section
    addSectionHeader(QObject::tr("JOIN"));
    addMenuItem("PartDesign_PartJoinConnect");
    addMenuItem("PartDesign_PartJoinEmbed");
    addMenuItem("PartDesign_PartJoinCutout");

    addSeparator();

    // Split section
    addSectionHeader(QObject::tr("SPLIT"));
    addMenuItem("PartDesign_PartBooleanFragments");
    addMenuItem("PartDesign_PartSliceApart");
    addMenuItem("PartDesign_PartSlice");
    addMenuItem("PartDesign_PartXor");

    addSeparator();

    // Compound section
    addSectionHeader(QObject::tr("COMPOUND"));
    addMenuItem("PartDesign_PartCompound");
    addMenuItem("PartDesign_PartExplodeCompound");
    addMenuItem("PartDesign_PartCompoundFilter");
    addMenuItem("PartDesign_PartToleranceSet");

    addSeparator();

    // Modeling section
    addSectionHeader(QObject::tr("MODELING"));
    addMenuItem("PartDesign_PartLoft");
    addMenuItem("PartDesign_PartSweep");
    addMenuItem("PartDesign_PartMakeFace");
    addMenuItem("PartDesign_PartShapeFromMesh");
    addMenuItem("PartDesign_PartSectionCut");

    addSeparator();

    // Copy section
    addSectionHeader(QObject::tr("COPY"));
    addMenuItem("PartDesign_PartSimpleCopy");
    addMenuItem("PartDesign_PartTransformedCopy");
    addMenuItem("PartDesign_PartElementCopy");

    addSeparator();

    // Helpers section
    addSectionHeader(QObject::tr("HELPERS"));
    addMenuItem("PartDesign_Body");
    addMenuItem("PartDesign_CompDatums");
    addMenuItem("PartDesign_Clone");
    addMenuItem("PartDesign_SubShapeBinder");

    addSeparator();

    // Extras section
    addSectionHeader(QObject::tr("EXTRAS"));
    addMenuItem("PartDesign_InvoluteGear");
    addMenuItem("PartDesign_Sprocket");
    addMenuItem("PartDesign_MigratePartMacros");

    return menu;
}

QMenu* PartDesignPartToolsWidget::buildModifyMenu()
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

    // Transform section
    addSectionHeader(QObject::tr("TRANSFORM"));
    addMenuItem("Std_TransformManip");
    addMenuItem("PartDesign_PartScale");
    addMenuItem("PartDesign_Mirrored");
    addMenuItem("PartDesign_LinearPattern");
    addMenuItem("PartDesign_PolarPattern");
    addMenuItem("PartDesign_MultiTransform");

    addSeparator();

    // Dress-Up section
    addSectionHeader(QObject::tr("DRESS-UP"));
    addMenuItem("PartDesign_Fillet");
    addMenuItem("PartDesign_Chamfer");
    addMenuItem("PartDesign_Draft");
    addMenuItem("PartDesign_Thickness");

    addSeparator();

    // Shape Operations section
    addSectionHeader(QObject::tr("SHAPE OPERATIONS"));
    addMenuItem("PartDesign_PartOffset");
    addMenuItem("PartDesign_PartOffset2D");
    addMenuItem("PartDesign_PartThickness");
    addMenuItem("PartDesign_PartRuledSurface");
    addMenuItem("PartDesign_PartCrossSections");
    addMenuItem("PartDesign_PartProjectionOnSurface");
    addMenuItem("PartDesign_PartEditAttachment");

    addSeparator();

    // Refine & Cleanup section
    addSectionHeader(QObject::tr("REFINE & CLEANUP"));
    addMenuItem("PartDesign_PartRefineShape");
    addMenuItem("PartDesign_PartReverseShape");
    addMenuItem("PartDesign_PartMakeSolid");
    addMenuItem("PartDesign_PartDefeaturing");

    return menu;
}

QMenu* PartDesignPartToolsWidget::buildInspectMenu()
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

    // Analysis section
    addSectionHeader(QObject::tr("ANALYSIS"));
    addMenuItem("PartDesign_PartCheckGeometry");
    addMenuItem("Std_Measure");
    addMenuItem("Sketcher_ValidateSketch");

    addSeparator();

    // Visualization section
    addSectionHeader(QObject::tr("VISUALIZATION"));
    addMenuItem("Materials_InspectAppearance");
    addMenuItem("Materials_InspectMaterial");

    return menu;
}

QIcon PartDesignPartToolsWidget::commandIcon(const char* cmdName)
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

#include "PartDesignPartToolsWidget.moc"
