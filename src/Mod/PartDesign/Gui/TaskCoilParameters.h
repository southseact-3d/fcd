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

#ifndef GUI_TASKVIEW_TaskCoilParameters_H
#define GUI_TASKVIEW_TaskCoilParameters_H

#include "TaskSketchBasedParameters.h"
#include "ViewProviderCoil.h"


namespace App
{
class Property;
}

namespace PartDesignGui
{
class Ui_TaskCoilParameters;


class TaskCoilParameters: public TaskSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskCoilParameters(ViewProviderCoil* CoilView, QWidget* parent = nullptr);
    ~TaskCoilParameters() override;

    void apply() override;

private:
    void fillAxisCombo(bool forceRefill = false);
    void addAxisToCombo(App::DocumentObject* linkObj, std::string linkSubname, QString itemText);
    void addPartAxes();
    int addCurrentLink();
    void assignToolTipsFromPropertyDocs();
    void adaptVisibilityToMode();

private Q_SLOTS:
    void onRadiusChanged(double);
    void onPitchChanged(double);
    void onHeightChanged(double);
    void onTurnsChanged(double);
    void onAngleChanged(double);
    void onSectionSizeChanged(double);
    void onSectionShapeChanged(int);
    void onSectionPositionChanged(int);
    void onAxisChanged(int);
    void onLeftHandedChanged(bool);
    void onReversedChanged(bool);
    void onModeChanged(int);

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void changeEvent(QEvent* e) override;
    bool updateView() const;
    void getReferenceAxis(App::DocumentObject*& obj, std::vector<std::string>& sub) const;
    void startReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base) override;

    App::PropertyLength* propRadius;
    App::PropertyLength* propPitch;
    App::PropertyLength* propHeight;
    App::PropertyFloatConstraint* propTurns;
    App::PropertyAngle* propAngle;
    App::PropertyLength* propSectionSize;
    App::PropertyEnumeration* propSectionShape;
    App::PropertyEnumeration* propSectionPosition;
    App::PropertyBool* propLeftHanded;
    App::PropertyBool* propReversed;
    App::PropertyLinkSub* propReferenceAxis;
    App::PropertyEnumeration* propMode;

private:
    void initializeCoil();
    void connectSlots();
    void updateUI();
    void assignProperties();
    void setValuesFromProperties();
    void bindProperties();

    QWidget* proxy;
    std::unique_ptr<Ui_TaskCoilParameters> ui;

    std::vector<std::unique_ptr<App::PropertyLinkSub>> axesInList;
};

/// simulation dialog for the TaskView
class TaskDlgCoilParameters: public TaskDlgSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgCoilParameters(ViewProviderCoil* CoilView);
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskCoilParameters_H
