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

#ifndef MESHPARTGUI_MESHTOODY_H
#define MESHPARTGUI_MESHTOODY_H

#include <QDialog>
#include <memory>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

class Ui_MeshToBody;

namespace MeshPartGui
{

class MeshToBody: public QDialog
{
    Q_OBJECT

public:
    explicit MeshToBody(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~MeshToBody() override;

    void perform();

public Q_SLOTS:
    void accept() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();

private:
    std::unique_ptr<Ui_MeshToBody> ui;
};

class TaskMeshToBody: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskMeshToBody();

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
    MeshToBody* widget;
};

}  // namespace MeshPartGui

#endif  // MESHPARTGUI_MESHTOODY_H
