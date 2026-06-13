// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten3D contributors                            *
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


#ifndef GUI_TASKVIEW_TaskThreadParameters_H
#define GUI_TASKVIEW_TaskThreadParameters_H

#include "TaskDressUpParameters.h"
#include "ViewProviderThread.h"

class Ui_TaskThreadParameters;
namespace PartDesign
{
class Thread;
}

namespace PartDesignGui
{

class TaskThreadParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskThreadParameters(ViewProviderDressUp* DressUpView, QWidget* parent = nullptr);
    ~TaskThreadParameters() override;

    void apply() override;

private Q_SLOTS:
    void onThreadTypeChanged(int);
    void onThreadSizeChanged(int);
    void onThreadDirectionChanged(int);
    void onThreadModeChanged(int);
    void onThreadLengthChanged(double);
    void onThreadPitchChanged(double);
    void onThreadDepthChanged(double);
    void onUseCustomClearanceChanged(bool);
    void onCustomClearanceChanged(double);
    void onTaperAngleChanged(double);
    void onRefDeleted() override;

protected:
    void setButtons(const selectionModes mode) override;
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    std::unique_ptr<Ui_TaskThreadParameters> ui;
};


class TaskDlgThreadParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgThreadParameters(ViewProviderThread* DressUpView);
    ~TaskDlgThreadParameters() override;

public:
    bool accept() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskThreadParameters_H
