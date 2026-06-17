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


#include "FemPartToolsWidget.h"

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

using namespace FemGui;

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
// FemGroupWidget — a labeled tool group (tools on top, label below)
// ============================================================================

class FemGui::FemGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FemGroupWidget(QWidget* parent = nullptr)
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
        connect(_labelButton, &QToolButton::clicked, this, &FemGroupWidget::showMenu);
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
// FemPartToolsWidget
// ============================================================================

FemPartToolsWidget::FemPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(4);

    _modelGroup = createModelGroup();
    _bcsGroup = createBCsGroup();
    _solveGroup = createSolveGroup();

    layout->addWidget(_modelGroup, 1);
    layout->addWidget(_bcsGroup, 1);
    layout->addWidget(_solveGroup, 1);
}

void FemPartToolsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int totalWidth = event->size().width();
    int groupWidth = totalWidth / 3;

    if (_modelGroup) {
        _modelGroup->updateVisibility(groupWidth);
    }
    if (_bcsGroup) {
        _bcsGroup->updateVisibility(groupWidth);
    }
    if (_solveGroup) {
        _solveGroup->updateVisibility(groupWidth);
    }
}

FemGroupWidget* FemPartToolsWidget::createModelGroup()
{
    QVector<const char*> commands = {
        "FEM_Analysis",
        "FEM_MaterialSolid",
        "FEM_MaterialFluid",
        "FEM_ElementGeometry1D",
        "FEM_ElementGeometry2D",
    };

    auto* group = createGroup(
        QStringLiteral("Model"),
        commands,
        buildModelMenu());

    return group;
}

FemGroupWidget* FemPartToolsWidget::createBCsGroup()
{
    QVector<const char*> commands = {
        "FEM_ConstraintFixed",
        "FEM_ConstraintDisplacement",
        "FEM_ConstraintForce",
        "FEM_ConstraintPressure",
        "FEM_ConstraintTemperature",
    };

    auto* group = createGroup(
        QStringLiteral("BCs"),
        commands,
        buildBCsMenu());

    return group;
}

FemGroupWidget* FemPartToolsWidget::createSolveGroup()
{
    QVector<const char*> commands = {
        "FEM_SolverCalculiX",
        "FEM_SolverElmer",
        "FEM_SolverRun",
        "FEM_ResultsPurge",
        "FEM_ResultShow",
    };

    auto* group = createGroup(
        QStringLiteral("Solve"),
        commands,
        buildSolveMenu());

    return group;
}

FemGroupWidget* FemPartToolsWidget::createGroup(
    const QString& label,
    const QVector<const char*>& visibleCommands,
    QMenu* menu)
{
    auto* group = new FemGroupWidget(this);
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

QToolButton* FemPartToolsWidget::createToolButton(const char* cmdName)
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

QMenu* FemPartToolsWidget::buildModelMenu()
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
    addMenuItem("FEM_Analysis");

    addSeparator();

    // Materials section
    addSectionHeader(QObject::tr("MATERIALS"));
    addMenuItem("FEM_MaterialSolid");
    addMenuItem("FEM_MaterialFluid");
    addMenuItem("FEM_MaterialMechanicalNonlinear");
    addMenuItem("FEM_MaterialReinforced");
    addMenuItem("FEM_MaterialEditor");

    addSeparator();

    // Element Geometry section
    addSectionHeader(QObject::tr("ELEMENT GEOMETRY"));
    addMenuItem("FEM_ElementGeometry1D");
    addMenuItem("FEM_ElementRotation1D");
    addMenuItem("FEM_ElementGeometry2D");
    addMenuItem("FEM_ElementFluid1D");

    return menu;
}

QMenu* FemPartToolsWidget::buildBCsMenu()
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

    // Mechanical section
    addSectionHeader(QObject::tr("MECHANICAL"));
    addMenuItem("FEM_ConstraintFixed");
    addMenuItem("FEM_ConstraintRigidBody");
    addMenuItem("FEM_ConstraintDisplacement");
    addMenuItem("FEM_ConstraintContact");
    addMenuItem("FEM_ConstraintTie");
    addMenuItem("FEM_ConstraintSpring");

    addSeparator();

    // Mechanical Loads section
    addSectionHeader(QObject::tr("MECHANICAL LOADS"));
    addMenuItem("FEM_ConstraintForce");
    addMenuItem("FEM_ConstraintPressure");
    addMenuItem("FEM_ConstraintCentrif");
    addMenuItem("FEM_ConstraintSelfWeight");

    addSeparator();

    // Geometrical section
    addSectionHeader(QObject::tr("GEOMETRICAL"));
    addMenuItem("FEM_ConstraintPlaneRotation");
    addMenuItem("FEM_ConstraintSectionPrint");
    addMenuItem("FEM_ConstraintTransform");

    addSeparator();

    // Thermal section
    addSectionHeader(QObject::tr("THERMAL"));
    addMenuItem("FEM_ConstraintInitialTemperature");
    addMenuItem("FEM_ConstraintHeatflux");
    addMenuItem("FEM_ConstraintTemperature");
    addMenuItem("FEM_ConstraintBodyHeatSource");

    addSeparator();

    // Fluid section
    addSectionHeader(QObject::tr("FLUID"));
    addMenuItem("FEM_ConstraintInitialFlowVelocity");
    addMenuItem("FEM_ConstraintInitialPressure");
    addMenuItem("FEM_ConstraintFlowVelocity");

    addSeparator();

    // Electromagnetic section
    addSectionHeader(QObject::tr("ELECTROMAGNETIC"));
    addMenuItem("FEM_CompEmConstraints");

    addSeparator();

    // Constants section
    addSectionHeader(QObject::tr("CONSTANTS"));
    addMenuItem("FEM_ConstantVacuumPermittivity");

    return menu;
}

QMenu* FemPartToolsWidget::buildSolveMenu()
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

    // Solvers section
    addSectionHeader(QObject::tr("SOLVERS"));
    addMenuItem("FEM_SolverCalculiX");
    addMenuItem("FEM_SolverElmer");
    addMenuItem("FEM_SolverMystran");
    addMenuItem("FEM_SolverZ88");

    addSeparator();

    // Equations section
    addSectionHeader(QObject::tr("EQUATIONS"));
    addMenuItem("FEM_CompMechEquations");
    addMenuItem("FEM_CompEmEquations");
    addMenuItem("FEM_EquationFlow");
    addMenuItem("FEM_EquationFlux");
    addMenuItem("FEM_EquationHeat");

    addSeparator();

    // Control section
    addSectionHeader(QObject::tr("CONTROL"));
    addMenuItem("FEM_SolverControl");
    addMenuItem("FEM_SolverRun");

    addSeparator();

    // Results section
    addSectionHeader(QObject::tr("RESULTS"));
    addMenuItem("FEM_ResultsPurge");
    addMenuItem("FEM_ResultShow");

    addSeparator();

    // Post Processing section
    addSectionHeader(QObject::tr("POST PROCESSING"));
    addMenuItem("FEM_PostApplyChanges");
    addMenuItem("FEM_PostPipelineFromResult");
    addMenuItem("FEM_PostBranchFilter");
    addMenuItem("FEM_PostFilterWarp");
    addMenuItem("FEM_PostFilterClipScalar");
    addMenuItem("FEM_PostFilterCutFunction");
    addMenuItem("FEM_PostFilterClipRegion");
    addMenuItem("FEM_PostFilterContours");
    addMenuItem("FEM_PostFilterDataAlongLine");
    addMenuItem("FEM_PostFilterLinearizedStresses");
    addMenuItem("FEM_PostFilterDataAtPoint");
    addMenuItem("FEM_PostFilterCalculator");
    addMenuItem("FEM_PostCreateFunctions");

    return menu;
}

QIcon FemPartToolsWidget::commandIcon(const char* cmdName)
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

#include "FemPartToolsWidget.moc"
