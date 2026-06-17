// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 FreeCAD Development Team                           *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of      *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 ***************************************************************************/


#ifndef GUI_TASKVIEW_TaskWoodPlankTextureParameters_H
#define GUI_TASKVIEW_TaskWoodPlankTextureParameters_H

#include "TaskDressUpParameters.h"
#include "ViewProviderWoodPlankTexture.h"

class Ui_TaskWoodPlankTextureParameters;

namespace PartDesignGui
{

class TaskWoodPlankTextureParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskWoodPlankTextureParameters(ViewProviderDressUp* DressUpView, QWidget* parent = nullptr);
    ~TaskWoodPlankTextureParameters() override;

    void apply() override;

private Q_SLOTS:
    void onPlankLengthChanged(double);
    void onPlankHeightChanged(double);
    void onPlankDepthChanged(double);
    void onGapWidthChanged(double);
    void onGapDepthChanged(double);
    void onOrientationChanged(int);
    void onEndOffsetChanged(int);
    void onScaleChanged(int);
    void onRefDeleted() override;

protected:
    void setButtons(const selectionModes mode) override;
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    std::unique_ptr<Ui_TaskWoodPlankTextureParameters> ui;
};


class TaskDlgWoodPlankTextureParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgWoodPlankTextureParameters(ViewProviderWoodPlankTexture* DressUpView);
    ~TaskDlgWoodPlankTextureParameters() override;

public:
    bool accept() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskWoodPlankTextureParameters_H
