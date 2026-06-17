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


#include "TechDrawPartToolsWidget.h"

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

using namespace TechDrawGui;

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
// TechDrawGroupWidget — a labeled tool group (tools on top, label below)
// ============================================================================

class TechDrawGui::TechDrawGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TechDrawGroupWidget(QWidget* parent = nullptr)
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
        connect(_labelButton, &QToolButton::clicked, this, &TechDrawGroupWidget::showMenu);
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
// TechDrawPartToolsWidget
// ============================================================================

TechDrawPartToolsWidget::TechDrawPartToolsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(4);

    _pageGroup = createPageGroup();
    _viewsGroup = createViewsGroup();
    _dimensionGroup = createDimensionGroup();

    layout->addWidget(_pageGroup, 1);
    layout->addWidget(_viewsGroup, 1);
    layout->addWidget(_dimensionGroup, 1);
}

void TechDrawPartToolsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int totalWidth = event->size().width();
    int groupWidth = totalWidth / 3;

    if (_pageGroup) {
        _pageGroup->updateVisibility(groupWidth);
    }
    if (_viewsGroup) {
        _viewsGroup->updateVisibility(groupWidth);
    }
    if (_dimensionGroup) {
        _dimensionGroup->updateVisibility(groupWidth);
    }
}

TechDrawGroupWidget* TechDrawPartToolsWidget::createPageGroup()
{
    QVector<const char*> commands = {
        "TechDraw_PageDefault",
        "TechDraw_PageTemplate",
        "TechDraw_RedrawPage",
    };

    auto* group = createGroup(
        QStringLiteral("Page"),
        commands,
        buildPageMenu());

    return group;
}

TechDrawGroupWidget* TechDrawPartToolsWidget::createViewsGroup()
{
    QVector<const char*> commands = {
        "TechDraw_View",
        "TechDraw_BrokenView",
        "TechDraw_SectionGroup",
        "TechDraw_DetailView",
        "TechDraw_DraftView",
    };

    auto* group = createGroup(
        QStringLiteral("Views"),
        commands,
        buildViewsMenu());

    return group;
}

TechDrawGroupWidget* TechDrawPartToolsWidget::createDimensionGroup()
{
    QVector<const char*> commands = {
        "TechDraw_CompDimensionTools",
        "TechDraw_Balloon",
        "TechDraw_DimensionRepair",
    };

    auto* group = createGroup(
        QStringLiteral("Dimension"),
        commands,
        buildDimensionMenu());

    return group;
}

TechDrawGroupWidget* TechDrawPartToolsWidget::createGroup(
    const QString& label,
    const QVector<const char*>& visibleCommands,
    QMenu* menu)
{
    auto* group = new TechDrawGroupWidget(this);
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

QToolButton* TechDrawPartToolsWidget::createToolButton(const char* cmdName)
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

QMenu* TechDrawPartToolsWidget::buildPageMenu()
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

    // Page Creation section
    addSectionHeader(QObject::tr("PAGE CREATION"));
    addMenuItem("TechDraw_PageDefault");
    addMenuItem("TechDraw_PageTemplate");
    addMenuItem("TechDraw_FillTemplateFields");

    addSeparator();

    // Page Operations section
    addSectionHeader(QObject::tr("PAGE OPERATIONS"));
    addMenuItem("TechDraw_RedrawPage");
    addMenuItem("TechDraw_PrintAll");

    addSeparator();

    // Export section
    addSectionHeader(QObject::tr("EXPORT"));
    addMenuItem("TechDraw_ExportPageSVG");
    addMenuItem("TechDraw_ExportPageDXF");

    return menu;
}

QMenu* TechDrawPartToolsWidget::buildViewsMenu()
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

    // Standard Views section
    addSectionHeader(QObject::tr("STANDARD VIEWS"));
    addMenuItem("TechDraw_View");
    addMenuItem("TechDraw_ActiveView");
    addMenuItem("TechDraw_BrokenView");

    addSeparator();

    // Section Views section
    addSectionHeader(QObject::tr("SECTION VIEWS"));
    addMenuItem("TechDraw_SectionGroup");
    addMenuItem("TechDraw_ComplexSection");

    addSeparator();

    // Detail and Projection section
    addSectionHeader(QObject::tr("DETAIL & PROJECTION"));
    addMenuItem("TechDraw_DetailView");
    addMenuItem("TechDraw_ProjectionGroup");
    addMenuItem("TechDraw_ClipGroup");

    addSeparator();

    // Other Workbenches section
    addSectionHeader(QObject::tr("OTHER WORKBENCHES"));
    addMenuItem("TechDraw_DraftView");
    addMenuItem("TechDraw_ArchView");
    addMenuItem("TechDraw_SpreadsheetView");

    addSeparator();

    // Additional section
    addSectionHeader(QObject::tr("ADDITIONAL"));
    addMenuItem("TechDraw_Symbol");
    addMenuItem("TechDraw_Image");
    addMenuItem("TechDraw_ShareView");
    addMenuItem("TechDraw_ToggleFrame");
    addMenuItem("TechDraw_ProjectShape");

    return menu;
}

QMenu* TechDrawPartToolsWidget::buildDimensionMenu()
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

    // Dimension Tools section
    addSectionHeader(QObject::tr("DIMENSION TOOLS"));
    addMenuItem("TechDraw_Dimension");
    addMenuItem("TechDraw_CompDimensionTools");
    addMenuItem("TechDraw_LengthDimension");
    addMenuItem("TechDraw_HorizontalDimension");
    addMenuItem("TechDraw_VerticalDimension");
    addMenuItem("TechDraw_RadiusDimension");
    addMenuItem("TechDraw_DiameterDimension");
    addMenuItem("TechDraw_AngleDimension");
    addMenuItem("TechDraw_3PtAngleDimension");
    addMenuItem("TechDraw_ExtentGroup");

    addSeparator();

    // Specialized Dimensions section
    addSectionHeader(QObject::tr("SPECIALIZED DIMENSIONS"));
    addMenuItem("TechDraw_AxoLengthDimension");
    addMenuItem("TechDraw_LandmarkDimension");
    addMenuItem("TechDraw_Balloon");

    addSeparator();

    // Dimension Management section
    addSectionHeader(QObject::tr("DIMENSION MANAGEMENT"));
    addMenuItem("TechDraw_DimensionRepair");

    addSeparator();

    // Annotations section
    addSectionHeader(QObject::tr("ANNOTATIONS"));
    addMenuItem("TechDraw_Annotation");
    addMenuItem("TechDraw_RichTextAnnotation");
    addMenuItem("TechDraw_LeaderLine");

    addSeparator();

    // Lines and Vertices section
    addSectionHeader(QObject::tr("LINES & VERTICES"));
    addMenuItem("TechDraw_CosmeticVertexGroup");
    addMenuItem("TechDraw_CenterLineGroup");
    addMenuItem("TechDraw_2PointCosmeticLine");
    addMenuItem("TechDraw_DecorateLine");
    addMenuItem("TechDraw_ShowAll");

    addSeparator();

    // Symbols section
    addSectionHeader(QObject::tr("SYMBOLS"));
    addMenuItem("TechDraw_WeldSymbol");
    addMenuItem("TechDraw_SurfaceFinishSymbols");
    addMenuItem("TechDraw_HoleShaftFit");

    return menu;
}

QIcon TechDrawPartToolsWidget::commandIcon(const char* cmdName)
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

#include "TechDrawPartToolsWidget.moc"
