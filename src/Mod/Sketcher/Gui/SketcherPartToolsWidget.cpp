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


#include "SketcherPartToolsWidget.h"

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

using namespace SketcherGui;

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
// SketcherGroupWidget — a labeled tool group (tools on top, label below)
// ============================================================================

class SketcherGui::SketcherGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SketcherGroupWidget(QWidget* parent = nullptr)
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

        QString resolvedLabelSheet = labelSheet;
        _labelButton->setStyleSheet(
            Gui::Application::Instance->replaceVariablesInQss(resolvedLabelSheet));

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

        QString resolvedToolSheet = groupToolSheet;
        btn->setStyleSheet(
            Gui::Application::Instance->replaceVariablesInQss(resolvedToolSheet));

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
        connect(_labelButton, &QToolButton::clicked, this, &SketcherGroupWidget::showMenu);
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
// SketcherPartToolsWidget
// ============================================================================

SketcherPartToolsWidget::SketcherPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(4);

    _sketchGroup = createSketchGroup();
    _geometryGroup = createGeometryGroup();
    _constrainGroup = createConstrainGroup();

    layout->addWidget(_sketchGroup, 1);
    layout->addWidget(_geometryGroup, 1);
    layout->addWidget(_constrainGroup, 1);
}

void SketcherPartToolsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int totalWidth = event->size().width();
    int groupWidth = totalWidth / 3;

    if (_sketchGroup) {
        _sketchGroup->updateVisibility(groupWidth);
    }
    if (_geometryGroup) {
        _geometryGroup->updateVisibility(groupWidth);
    }
    if (_constrainGroup) {
        _constrainGroup->updateVisibility(groupWidth);
    }
}

SketcherGroupWidget* SketcherPartToolsWidget::createSketchGroup()
{
    QVector<const char*> commands = {
        "Sketcher_NewSketch",
        "Sketcher_EditSketch",
        "Sketcher_MapSketch",
        "Sketcher_ReorientSketch",
    };

    auto* group = createGroup(
        QStringLiteral("Sketch"),
        commands,
        buildSketchMenu());

    return group;
}

SketcherGroupWidget* SketcherPartToolsWidget::createGeometryGroup()
{
    QVector<const char*> commands = {
        "Sketcher_CreatePoint",
        "Sketcher_CreateLine",
        "Sketcher_CreateArc",
        "Sketcher_CreateCircle",
        "Sketcher_CreateRectangle",
    };

    auto* group = createGroup(
        QStringLiteral("Geometry"),
        commands,
        buildGeometryMenu());

    return group;
}

SketcherGroupWidget* SketcherPartToolsWidget::createConstrainGroup()
{
    QVector<const char*> commands = {
        "Sketcher_ConstrainDistance",
        "Sketcher_ConstrainHorizontal",
        "Sketcher_ConstrainVertical",
        "Sketcher_ConstrainParallel",
        "Sketcher_ConstrainPerpendicular",
    };

    auto* group = createGroup(
        QStringLiteral("Constrain"),
        commands,
        buildConstrainMenu());

    return group;
}

SketcherGroupWidget* SketcherPartToolsWidget::createGroup(
    const QString& label,
    const QVector<const char*>& visibleCommands,
    QMenu* menu)
{
    auto* group = new SketcherGroupWidget(this);
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

QToolButton* SketcherPartToolsWidget::createToolButton(const char* cmdName)
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

    QString resolvedToolSheet = groupToolSheet;
    btn->setStyleSheet(
        Gui::Application::Instance->replaceVariablesInQss(resolvedToolSheet));

    return btn;
}

QMenu* SketcherPartToolsWidget::buildSketchMenu()
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

    // Sketch management section
    addSectionHeader(QObject::tr("SKETCH"));
    addMenuItem("Sketcher_NewSketch");
    addMenuItem("Sketcher_EditSketch");
    addMenuItem("Sketcher_MapSketch");
    addMenuItem("Sketcher_ReorientSketch");

    addSeparator();

    // Sketch edit mode section
    addSectionHeader(QObject::tr("EDIT MODE"));
    addMenuItem("Sketcher_LeaveSketch");
    addMenuItem("Sketcher_ViewSketch");
    addMenuItem("Sketcher_ViewSection");

    addSeparator();

    // Sketch validation section
    addSectionHeader(QObject::tr("VALIDATION"));
    addMenuItem("Sketcher_ValidateSketch");
    addMenuItem("Sketcher_MergeSketches");
    addMenuItem("Sketcher_MirrorSketch");

    return menu;
}

QMenu* SketcherPartToolsWidget::buildGeometryMenu()
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

    // Points and lines section
    addSectionHeader(QObject::tr("LINES"));
    addMenuItem("Sketcher_CreatePoint");
    addMenuItem("Sketcher_CreatePolyline");
    addMenuItem("Sketcher_CreateLine");

    addSeparator();

    // Arcs and circles section
    addSectionHeader(QObject::tr("ARCS & CIRCLES"));
    addMenuItem("Sketcher_CreateArc");
    addMenuItem("Sketcher_Create3PointArc");
    addMenuItem("Sketcher_CreateCircle");
    addMenuItem("Sketcher_Create3PointCircle");

    addSeparator();

    // Conics section
    addSectionHeader(QObject::tr("CONICS"));
    addMenuItem("Sketcher_CreateArcOfEllipse");
    addMenuItem("Sketcher_CreateArcOfHyperbola");
    addMenuItem("Sketcher_CreateArcOfParabola");
    addMenuItem("Sketcher_CreateEllipseByCenter");
    addMenuItem("Sketcher_CreateEllipseBy3Points");

    addSeparator();

    // Rectangles section
    addSectionHeader(QObject::tr("RECTANGLES"));
    addMenuItem("Sketcher_CreateRectangle");
    addMenuItem("Sketcher_CreateRectangle_Center");
    addMenuItem("Sketcher_CreateOblong");

    addSeparator();

    // Polygons section
    addSectionHeader(QObject::tr("POLYGONS"));
    addMenuItem("Sketcher_CreateTriangle");
    addMenuItem("Sketcher_CreateSquare");
    addMenuItem("Sketcher_CreatePentagon");
    addMenuItem("Sketcher_CreateHexagon");
    addMenuItem("Sketcher_CreateHeptagon");
    addMenuItem("Sketcher_CreateOctagon");
    addMenuItem("Sketcher_CreateRegularPolygon");

    addSeparator();

    // Slots section
    addSectionHeader(QObject::tr("SLOTS"));
    addMenuItem("Sketcher_CreateSlot");
    addMenuItem("Sketcher_CreateArcSlot");

    addSeparator();

    // B-splines section
    addSectionHeader(QObject::tr("B-SPLINES"));
    addMenuItem("Sketcher_CreateBSpline");
    addMenuItem("Sketcher_CreatePeriodicBSpline");
    addMenuItem("Sketcher_CreateBSplineByInterpolation");
    addMenuItem("Sketcher_CreatePeriodicBSplineByInterpolation");

    addSeparator();

    // Fillets and chamfers section
    addSectionHeader(QObject::tr("FILLETS & CHAMFERS"));
    addMenuItem("Sketcher_CreateFillet");
    addMenuItem("Sketcher_CreateChamfer");

    addSeparator();

    // Curve editing section
    addSectionHeader(QObject::tr("CURVE EDITING"));
    addMenuItem("Sketcher_Trimming");
    addMenuItem("Sketcher_Extend");
    addMenuItem("Sketcher_Split");

    addSeparator();

    // Construction mode
    addMenuItem("Sketcher_ToggleConstruction");

    return menu;
}

QMenu* SketcherPartToolsWidget::buildConstrainMenu()
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

    // Geometric constraints section
    addSectionHeader(QObject::tr("GEOMETRIC"));
    addMenuItem("Sketcher_ConstrainCoincidentUnified");
    addMenuItem("Sketcher_ConstrainCoincident");
    addMenuItem("Sketcher_ConstrainPointOnObject");
    addMenuItem("Sketcher_ConstrainHorizontal");
    addMenuItem("Sketcher_ConstrainVertical");
    addMenuItem("Sketcher_ConstrainParallel");
    addMenuItem("Sketcher_ConstrainPerpendicular");
    addMenuItem("Sketcher_ConstrainTangent");
    addMenuItem("Sketcher_ConstrainEqual");
    addMenuItem("Sketcher_ConstrainSymmetric");
    addMenuItem("Sketcher_ConstrainBlock");

    addSeparator();

    // Dimensional constraints section
    addSectionHeader(QObject::tr("DIMENSIONAL"));
    addMenuItem("Sketcher_Dimension");
    addMenuItem("Sketcher_ConstrainDistanceX");
    addMenuItem("Sketcher_ConstrainDistanceY");
    addMenuItem("Sketcher_ConstrainDistance");
    addMenuItem("Sketcher_ConstrainRadiam");
    addMenuItem("Sketcher_ConstrainRadius");
    addMenuItem("Sketcher_ConstrainDiameter");
    addMenuItem("Sketcher_ConstrainAngle");
    addMenuItem("Sketcher_ConstrainLock");
    addMenuItem("Sketcher_ConstrainSnellsLaw");

    addSeparator();

    // Constraint tools section
    addSectionHeader(QObject::tr("TOOLS"));
    addMenuItem("Sketcher_ToggleDrivingConstraint");
    addMenuItem("Sketcher_ToggleActiveConstraint");

    return menu;
}

QIcon SketcherPartToolsWidget::commandIcon(const char* cmdName)
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

#include "SketcherPartToolsWidget.moc"
