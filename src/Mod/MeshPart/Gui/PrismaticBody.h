// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD contributors                          *
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
 *   Contributors:                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef MESHPARTGUI_PRISMATICBODY_H
#define MESHPARTGUI_PRISMATICBODY_H

#include <QDialog>
#include <memory>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

namespace MeshPartGui
{

class Ui_PrismaticBody;
class PrismaticBody: public QDialog
{
    Q_OBJECT

public:
    explicit PrismaticBody(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~PrismaticBody() override;

    void perform();

public Q_SLOTS:
    void accept() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();
    void onDirectionChanged();

private:
    std::unique_ptr<Ui_PrismaticBody> ui;
};

class TaskPrismaticBody: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskPrismaticBody();

public:
    void open() override;
    void clicked(int) override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    PrismaticBody* widget;
};

}  // namespace MeshPartGui

#endif  // MESHPARTGUI_PRISMATICBODY_H
