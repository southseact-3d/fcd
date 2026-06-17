// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD contributors                           *
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


#ifndef PARTGUI_TASKFACEOFFSET_H
#define PARTGUI_TASKFACEOFFSET_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>

namespace Part
{
class FaceOffset;
}
namespace PartGui
{

class FaceOffsetWidget: public QWidget
{
    Q_OBJECT

public:
    explicit FaceOffsetWidget(Part::FaceOffset*, QWidget* parent = nullptr);
    ~FaceOffsetWidget() override;

    bool accept();
    bool reject();
    Part::FaceOffset* getObject() const;

private:
    void setupConnections();
    void onSpinOffsetValueChanged(double);
    void onJoinTypeActivated(int);
    void onIntersectionToggled(bool);
    void onFacesButtonToggled(bool);
    void onUpdateViewToggled(bool);

private:
    void changeEvent(QEvent* e) override;

private:
    class Private;
    Private* d;
};

class TaskFaceOffset: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskFaceOffset(Part::FaceOffset*);
    ~TaskFaceOffset() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;
    Part::FaceOffset* getObject() const;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    FaceOffsetWidget* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKFACEOFFSET_H
