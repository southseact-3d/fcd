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


#include "ToolSettingsPanel.h"

#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QStyle>

#include "MainWindow.h"
#include "OverlayManager.h"
#include "TaskView/TaskDialog.h"
#include "TaskView/TaskView.h"

using namespace Gui;

static const int PanelMaxWidth = 360;
static const int PanelMinWidth = 240;
static const int PanelMargin = 8;
static const int PanelSpacing = 4;

ToolSettingsPanel::ToolSettingsPanel(QWidget* viewportWidget)
    : QWidget(viewportWidget)
    , _viewportWidget(viewportWidget)
    , _activeDialog(nullptr)
    , _mainLayout(nullptr)
    , _scrollArea(nullptr)
    , _contentWidget(nullptr)
    , _contentLayout(nullptr)
    , _buttonBar(nullptr)
    , _buttonBox(nullptr)
{
    setupUi();
    hide();

    if (_viewportWidget) {
        _viewportWidget->installEventFilter(this);
    }
}

ToolSettingsPanel::~ToolSettingsPanel()
{
    if (_viewportWidget) {
        _viewportWidget->removeEventFilter(this);
    }
}

void ToolSettingsPanel::setupUi()
{
    setObjectName(QStringLiteral("ToolSettingsPanel"));
    setMinimumWidth(PanelMinWidth);
    setMaximumWidth(PanelMaxWidth);

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->setContentsMargins(PanelMargin, PanelMargin, PanelMargin, PanelMargin);
    _mainLayout->setSpacing(PanelSpacing);

    // Scroll area for dialog content
    _scrollArea = new QScrollArea(this);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _scrollArea->setFrameShape(QFrame::NoFrame);
    _scrollArea->setStyleSheet(QStringLiteral(
        "QScrollArea { background-color: transparent; border: none; }"
    ));

    _contentWidget = new QWidget();
    _contentLayout = new QVBoxLayout(_contentWidget);
    _contentLayout->setContentsMargins(0, 0, 0, 0);
    _contentLayout->setSpacing(PanelSpacing);
    _contentLayout->addStretch();
    _scrollArea->setWidget(_contentWidget);

    _mainLayout->addWidget(_scrollArea, 1);

    // Button bar
    _buttonBar = new QFrame(this);
    _buttonBar->setFrameShape(QFrame::StyledPanel);
    auto* buttonLayout = new QHBoxLayout(_buttonBar);
    buttonLayout->setContentsMargins(4, 4, 4, 4);

    _buttonBox = new QDialogButtonBox(_buttonBar);
    _buttonBox->setCenterButtons(true);
    connect(_buttonBox, &QDialogButtonBox::clicked, this, &ToolSettingsPanel::onButtonClicked);

    buttonLayout->addWidget(_buttonBox);
    _mainLayout->addWidget(_buttonBar, 0);
}

void ToolSettingsPanel::showDialog(TaskView::TaskDialog* dlg)
{
    if (!dlg) {
        return;
    }

    // If we already have a dialog, clean up first
    if (_activeDialog) {
        clearContents();
    }

    _activeDialog = dlg;

    // Extract content widgets from the dialog
    const std::vector<QWidget*>& content = dlg->getDialogContent();
    for (QWidget* w : content) {
        if (w) {
            _contentLayout->insertWidget(_contentLayout->count() - 1, w);
        }
    }

    // Set up button box
    _buttonBox->setStandardButtons(dlg->getStandardButtons());
    TaskView::TaskDialogAttorney::setButtonBox(dlg, _buttonBox);
    dlg->modifyStandardButtons(_buttonBox);

    _buttonBar->setVisible(_buttonBox->standardButtons() != QDialogButtonBox::NoButton);

    // Notify the dialog it is open
    dlg->open();

    // Position and show
    updatePosition();
    show();
    raise();

    // Update main window actions (e.g. disable certain commands while dialog is open)
    if (auto* mainWin = getMainWindow()) {
        mainWin->updateActions();
    }

    OverlayManager::instance()->refresh();
}

void ToolSettingsPanel::hidePanel()
{
    clearContents();
    hide();

    // Update main window actions
    if (auto* mainWin = getMainWindow()) {
        mainWin->updateActions();
    }

    OverlayManager::instance()->refresh();
}

bool ToolSettingsPanel::hasActiveDialog() const
{
    return _activeDialog != nullptr;
}

TaskView::TaskDialog* ToolSettingsPanel::activeDialog() const
{
    return _activeDialog;
}

void ToolSettingsPanel::clearContents()
{
    // Remove all widgets from the content layout except the trailing stretch
    while (_contentLayout->count() > 1) {
        QLayoutItem* item = _contentLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }

    _activeDialog = nullptr;
}

void ToolSettingsPanel::updatePosition()
{
    if (!_viewportWidget) {
        return;
    }

    int vpWidth = _viewportWidget->width();
    int vpHeight = _viewportWidget->height();

    // Calculate desired size
    int panelWidth = qMin(PanelMaxWidth, vpWidth - 2 * PanelMargin);
    panelWidth = qMax(panelWidth, PanelMinWidth);
    int panelHeight = qMin(height(), vpHeight - 2 * PanelMargin);

    // Position in bottom-right corner
    int x = vpWidth - panelWidth - PanelMargin;
    int y = vpHeight - panelHeight - PanelMargin;

    setGeometry(x, y, panelWidth, panelHeight);
}

bool ToolSettingsPanel::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _viewportWidget && event->type() == QEvent::Resize) {
        updatePosition();
    }
    return QWidget::eventFilter(obj, event);
}

void ToolSettingsPanel::onButtonClicked(QAbstractButton* button)
{
    if (!_activeDialog) {
        return;
    }

    auto standardButton = _buttonBox->standardButton(button);
    if (standardButton & QDialogButtonBox::Ok) {
        Q_EMIT dialogAccepted();
    }
    else if (standardButton & QDialogButtonBox::Cancel) {
        Q_EMIT dialogRejected();
    }
    else {
        // Find the button role index for custom buttons
        int index = _buttonBox->buttons().indexOf(button);
        Q_EMIT dialogClicked(index);
    }
}

#include "moc_ToolSettingsPanel.cpp"
