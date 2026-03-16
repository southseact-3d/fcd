/***************************************************************************
 *   Copyright (c) 2023 Your Name <your.email@example.com>                 *
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

#include "GlobalProgressBarOverlay.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QScreen>

namespace Gui
{

GlobalProgressBarOverlay::GlobalProgressBarOverlay(QWidget* parent)
    : QWidget(parent)
    , progressBar(nullptr)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    progressBar = new QProgressBar(this);
    progressBar->setFixedWidth(200);
    progressBar->setFixedHeight(20);
    progressBar->setTextVisible(true);
    progressBar->setAlignment(Qt::AlignCenter);

    progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "   background-color: #f0f0f0;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #05B8CC;"
        "   width: 10px;"
        "}"
    );

    layout->addWidget(progressBar);

    updatePosition();
}

GlobalProgressBarOverlay::~GlobalProgressBarOverlay()
{
}

void GlobalProgressBarOverlay::showOverlay()
{
    show();
    raise();
    activateWindow();
}

void GlobalProgressBarOverlay::hideOverlay()
{
    hide();
}

void GlobalProgressBarOverlay::setRange(int minimum, int maximum)
{
    if (progressBar)
        progressBar->setRange(minimum, maximum);
}

void GlobalProgressBarOverlay::setValue(int value)
{
    if (progressBar)
        progressBar->setValue(value);
}

void GlobalProgressBarOverlay::reset()
{
    if (progressBar)
        progressBar->reset();
}

void GlobalProgressBarOverlay::updatePosition()
{
    QScreen* screen = QApplication::primaryScreen();
    if (!screen)
        return;
        
    QRect screenGeometry = screen->geometry();

    int x = 20;
    int y = screenGeometry.height() - height() - 20;

    move(x, y);
}

void GlobalProgressBarOverlay::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    updatePosition();
}

void GlobalProgressBarOverlay::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
}

} // namespace Gui
