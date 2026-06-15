// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2013 Jan Rheinländer                                    *
 *                                   <jrheinlaender@users.sourceforge.net> *
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


#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>


#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Selection/Selection.h>
#include <Gui/Tools.h>
#include <Gui/ViewProvider.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/App/FeatureBoolean.h>

#include "ui_TaskBooleanParameters.h"
#include "TaskBooleanParameters.h"
#include "Utils.h"


using namespace PartDesignGui;
using namespace Gui;

/* TRANSLATOR PartDesignGui::TaskBooleanParameters */

TaskBooleanParameters::TaskBooleanParameters(ViewProviderBoolean* BooleanView, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("PartDesign_Boolean"), tr("Boolean Parameters"), true, parent)
    , ui(new Ui_TaskBooleanParameters)
    , BooleanView(BooleanView)
    , selectionMode(none)
{
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    // Connect signals
    connect(ui->buttonChangeTarget, &QPushButton::clicked,
            this, &TaskBooleanParameters::onButtonChangeTarget);
    connect(ui->buttonRemoveTool, &QPushButton::clicked,
            this, &TaskBooleanParameters::onButtonRemoveTool);
    connect(ui->radioJoin, &QRadioButton::toggled,
            this, [this](bool checked) { if (checked) onTypeChanged(0); });
    connect(ui->radioCut, &QRadioButton::toggled,
            this, [this](bool checked) { if (checked) onTypeChanged(1); });
    connect(ui->radioIntersect, &QRadioButton::toggled,
            this, [this](bool checked) { if (checked) onTypeChanged(2); });
    connect(ui->checkKeepTools, &QCheckBox::checkStateChanged,
            this, &TaskBooleanParameters::onKeepToolsChanged);

    // Context menu for removing items
    QAction* action = new QAction(tr("Remove"), this);
    action->setShortcut(Gui::QtTools::deleteKeySequence());
    action->setShortcutVisibleInContextMenu(true);
    ui->listWidgetBodies->addAction(action);
    connect(action, &QAction::triggered, this, &TaskBooleanParameters::onButtonRemoveTool);
    ui->listWidgetBodies->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Selection timer to detect when user clicks in 3D view
    selectionTimer = new QTimer(this);
    selectionTimer->setSingleShot(true);
    selectionTimer->setInterval(100);
    connect(selectionTimer, &QTimer::timeout, this, &TaskBooleanParameters::onSelectionModeTimer);

    // Initialize from current feature state
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();

    // Set target body label
    updateTargetLabel();

    // Populate tool bodies list
    std::vector<App::DocumentObject*> bodies = pcBoolean->Group.getValues();
    for (auto body : bodies) {
        QListWidgetItem* item = new QListWidgetItem(ui->listWidgetBodies);
        item->setText(QString::fromUtf8(body->Label.getValue()));
        item->setData(Qt::UserRole, QString::fromLatin1(body->getNameInDocument()));
    }

    // Set operation type
    int index = pcBoolean->Type.getValue();
    switch (index) {
        case 0: ui->radioJoin->setChecked(true); break;
        case 1: ui->radioCut->setChecked(true); break;
        case 2: ui->radioIntersect->setChecked(true); break;
        default: ui->radioJoin->setChecked(true); break;
    }

    // Set Keep Tools checkbox
    ui->checkKeepTools->setChecked(pcBoolean->KeepTools.getValue());

    // Start in tool selection mode (most common action)
    enterSelectionMode();
}

TaskBooleanParameters::~TaskBooleanParameters()
{
    try {
        Gui::Selection().clearSelection();
        Gui::Selection().rmvSelectionGate();
    }
    catch (const Base::Exception& e) {
        e.reportException();
    }
}

void TaskBooleanParameters::enterSelectionMode()
{
    if (selectionMode == toolSelect) {
        return; // already in tool select mode
    }
    selectionMode = toolSelect;
    Gui::Selection().clearSelection();
    ui->labelHint->setText(tr("Click bodies in 3D view to add as tools."));
}

void TaskBooleanParameters::exitSelectionMode()
{
    selectionMode = none;
    Gui::Selection().rmvSelectionGate();
    ui->labelHint->setText(tr("Click bodies in 3D view to add as tools."));
}

void TaskBooleanParameters::updateTargetLabel()
{
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    App::DocumentObject* baseFeature = pcBoolean->BaseFeature.getValue();

    if (baseFeature) {
        ui->labelTargetBody->setText(QString::fromUtf8(baseFeature->Label.getValue()));
    }
    else {
        // Try to get the active body
        PartDesign::Body* activeBody = PartDesignGui::getBody(false);
        if (activeBody) {
            ui->labelTargetBody->setText(QString::fromUtf8(activeBody->Label.getValue()));
        }
        else {
            ui->labelTargetBody->setText(tr("(no body)"));
        }
    }
}

bool TaskBooleanParameters::isBodyInTools(App::DocumentObject* body) const
{
    for (int i = 0; i < ui->listWidgetBodies->count(); i++) {
        QString name = ui->listWidgetBodies->item(i)->data(Qt::UserRole).toString();
        if (name == QLatin1String(body->getNameInDocument())) {
            return true;
        }
    }
    return false;
}

void TaskBooleanParameters::addToolBody(App::DocumentObject* body)
{
    if (!body || isBodyInTools(body)) {
        return;
    }

    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    std::vector<App::DocumentObject*> bodies = pcBoolean->Group.getValues();
    bodies.push_back(body);
    pcBoolean->Group.setValues(bodies);

    QListWidgetItem* item = new QListWidgetItem(ui->listWidgetBodies);
    item->setText(QString::fromUtf8(body->Label.getValue()));
    item->setData(Qt::UserRole, QString::fromLatin1(body->getNameInDocument()));

    pcBoolean->getDocument()->recomputeFeature(pcBoolean);

    // Hide the newly added body
    Gui::ViewProviderDocumentObject* vp = dynamic_cast<Gui::ViewProviderDocumentObject*>(
        Gui::Application::Instance->getViewProvider(body)
    );
    if (vp) {
        vp->hide();
    }

    // Hide the boolean result if first tool body
    if (bodies.size() == 1) {
        BooleanView->show();
    }
}

void TaskBooleanParameters::removeToolBody(int row)
{
    if (row < 0 || row >= ui->listWidgetBodies->count()) {
        return;
    }

    QListWidgetItem* item = ui->listWidgetBodies->item(row);
    QString internalName = item->data(Qt::UserRole).toString();

    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    std::vector<App::DocumentObject*> bodies = pcBoolean->Group.getValues();

    for (auto it = bodies.begin(); it != bodies.end(); ++it) {
        if (internalName == QLatin1String((*it)->getNameInDocument())) {
            App::DocumentObject* removedBody = *it;
            bodies.erase(it);
            pcBoolean->Group.setValues(bodies);

            // Show the removed body again
            Gui::ViewProviderDocumentObject* vp = dynamic_cast<Gui::ViewProviderDocumentObject*>(
                Gui::Application::Instance->getViewProvider(removedBody)
            );
            if (vp) {
                vp->show();
            }
            break;
        }
    }

    ui->listWidgetBodies->model()->removeRow(row);
    pcBoolean->getDocument()->recomputeFeature(pcBoolean);

    // If no more tool bodies, hide the boolean result
    if (bodies.empty()) {
        BooleanView->hide();
    }
}

void TaskBooleanParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (selectionMode == none) {
        return;
    }

    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        if (strcmp(msg.pDocName, BooleanView->getObject()->getDocument()->getName()) != 0) {
            return;
        }

        PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
        std::string body(msg.pObjectName);
        if (body.empty()) {
            return;
        }
        App::DocumentObject* pcBody = pcBoolean->getDocument()->getObject(body.c_str());
        if (!pcBody) {
            return;
        }

        // If the selected object is not a body then get the body it is part of
        if (!pcBody->isDerivedFrom<PartDesign::Body>()) {
            pcBody = PartDesign::Body::findBodyOf(pcBody);
            // Standalone Part::Feature objects (e.g. Part::Box, Part::Cylinder)
            // are not inside any Body. Accept them directly as the tool object.
            if (!pcBody) {
                pcBody = pcBoolean->getDocument()->getObject(body.c_str());
                if (!pcBody || !pcBody->isDerivedFrom<Part::Feature>()) {
                    return;
                }
            }
        }

        if (selectionMode == targetSelect) {
            // Set the clicked body as the target
            if (pcBody != pcBoolean->BaseFeature.getValue()) {
                // Don't allow target to be a tool body
                if (isBodyInTools(pcBody)) {
                    return;
                }

                pcBoolean->BaseFeature.setValue(pcBody);
                updateTargetLabel();
                pcBoolean->getDocument()->recomputeFeature(pcBoolean);
            }
            exitSelectionMode();
        }
        else if (selectionMode == toolSelect) {
            // Don't allow target body to be added as tool
            App::DocumentObject* baseFeature = pcBoolean->BaseFeature.getValue();
            if (pcBody == baseFeature) {
                return;
            }

            addToolBody(pcBody);

            // Stay in toolSelect mode so user can keep adding bodies
            Gui::Selection().clearSelection();
        }
    }
}

void TaskBooleanParameters::onButtonChangeTarget()
{
    selectionMode = targetSelect;
    Gui::Selection().clearSelection();
    ui->labelHint->setText(tr("Click a body in 3D view to set as target."));
}

void TaskBooleanParameters::onButtonRemoveTool()
{
    int row = ui->listWidgetBodies->currentRow();
    if (row >= 0) {
        removeToolBody(row);
    }
}

void TaskBooleanParameters::onTypeChanged(int index)
{
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();

    switch (index) {
        case 0:
            pcBoolean->Type.setValue("Join");
            break;
        case 1:
            pcBoolean->Type.setValue("Cut");
            break;
        case 2:
            pcBoolean->Type.setValue("Intersect");
            break;
        default:
            pcBoolean->Type.setValue("Join");
    }

    QApplication::processEvents();
    pcBoolean->getDocument()->recomputeFeature(pcBoolean);
}

void TaskBooleanParameters::onKeepToolsChanged(int state)
{
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    pcBoolean->KeepTools.setValue(state == Qt::Checked);
    pcBoolean->getDocument()->recomputeFeature(pcBoolean);
}

void TaskBooleanParameters::onSelectionModeTimer()
{
    // Periodic check when in selection mode (optional, for future use)
}

bool TaskBooleanParameters::eventFilter(QObject* obj, QEvent* event)
{
    return TaskBox::eventFilter(obj, event);
}

const std::vector<std::string> TaskBooleanParameters::getBodies() const
{
    std::vector<std::string> result;
    for (int i = 0; i < ui->listWidgetBodies->count(); i++) {
        result.push_back(ui->listWidgetBodies->item(i)->data(Qt::UserRole).toString().toStdString());
    }
    return result;
}

int TaskBooleanParameters::getType() const
{
    if (ui->radioJoin->isChecked()) return 0;
    if (ui->radioCut->isChecked()) return 1;
    if (ui->radioIntersect->isChecked()) return 2;
    return 0;
}

bool TaskBooleanParameters::getKeepTools() const
{
    return ui->checkKeepTools->isChecked();
}

std::string TaskBooleanParameters::getTargetBody() const
{
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    App::DocumentObject* baseFeature = pcBoolean->BaseFeature.getValue();
    if (baseFeature) {
        return baseFeature->getNameInDocument();
    }
    return "";
}

void TaskBooleanParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgBooleanParameters::TaskDlgBooleanParameters(ViewProviderBoolean* BooleanView)
    : TaskDlgFeatureParameters(BooleanView)
    , BooleanView(BooleanView)
{
    assert(BooleanView);
    parameter = new TaskBooleanParameters(BooleanView);

    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgBooleanParameters::~TaskDlgBooleanParameters() = default;

//==== calls from the TaskView ===============================================================


void TaskDlgBooleanParameters::open()
{}

void TaskDlgBooleanParameters::clicked(int)
{}

bool TaskDlgBooleanParameters::accept()
{
    auto obj = BooleanView->getObject();
    if (!obj || !obj->isAttachedToDocument()) {
        return false;
    }
    BooleanView->Visibility.setValue(true);

    bool keepTools = false;

    try {
        // Set the target body (BaseFeature)
        std::string target = parameter->getTargetBody();
        if (target.empty()) {
            // Fallback: use the active body as target
            PartDesign::Body* activeBody = PartDesignGui::getBody(false);
            if (activeBody) {
                target = activeBody->getNameInDocument();
            }
        }
        if (!target.empty()) {
            std::stringstream str;
            str << Gui::Command::getObjectCmd(obj) << ".BaseFeature = App.getDocument('"
                << obj->getDocument()->getName() << "').getObject('" << target << "')";
            Gui::Command::runCommand(Gui::Command::Doc, str.str().c_str());
        }

        // Set tool bodies
        std::vector<std::string> bodies = parameter->getBodies();
        if (bodies.empty()) {
            QMessageBox::warning(parameter, tr("Empty body list"), tr("Please select at least one tool body."));
            return false;
        }
        std::stringstream str;
        str << Gui::Command::getObjectCmd(obj) << ".setObjects( [";
        for (const auto& body : bodies) {
            str << "App.getDocument('" << obj->getDocument()->getName() << "').getObject('" << body
                << "'),";
        }
        str << "])";
        Gui::Command::runCommand(Gui::Command::Doc, str.str().c_str());

        // Set Keep Tools
        keepTools = parameter->getKeepTools();
        std::stringstream keepStr;
        keepStr << Gui::Command::getObjectCmd(obj) << ".KeepTools = " << (keepTools ? "True" : "False");
        Gui::Command::runCommand(Gui::Command::Doc, keepStr.str().c_str());
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(
            parameter,
            tr("Boolean: Accept: Input error"),
            QCoreApplication::translate("Exception", e.what())
        );
        return false;
    }

    // Set operation type
    FCMD_OBJ_CMD(obj, "Type = " << parameter->getType());

    // Show/hide bodies based on KeepTools setting
    PartDesign::Boolean* pcBoolean = BooleanView->getObject<PartDesign::Boolean>();
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        if (!keepTools) {
            // Hide tool bodies (they are consumed)
            std::vector<App::DocumentObject*> toolBodies = pcBoolean->Group.getValues();
            for (auto body : toolBodies) {
                Gui::ViewProviderDocumentObject* vp = dynamic_cast<Gui::ViewProviderDocumentObject*>(
                    Gui::Application::Instance->getViewProvider(body)
                );
                if (vp) {
                    vp->hide();
                }
            }
            // Hide the target body too (it's consumed into the result)
            if (pcBoolean->BaseFeature.getValue()) {
                Gui::ViewProviderDocumentObject* vp = dynamic_cast<Gui::ViewProviderDocumentObject*>(
                    Gui::Application::Instance->getViewProvider(pcBoolean->BaseFeature.getValue())
                );
                if (vp) {
                    vp->hide();
                }
            }
        }
        // Show the boolean result
        BooleanView->show();
    }

    Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");
    Gui::Command::commitCommand();

    return true;
}

bool TaskDlgBooleanParameters::reject()
{
    // Show all bodies again
    PartDesign::Boolean* obj = BooleanView->getObject<PartDesign::Boolean>();
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        if (obj->BaseFeature.getValue()) {
            doc->setShow(obj->BaseFeature.getValue()->getNameInDocument());
        }
        std::vector<App::DocumentObject*> bodies = obj->Group.getValues();
        for (auto body : bodies) {
            doc->setShow(body->getNameInDocument());
        }
    }

    // roll back the done things
    Gui::Command::abortCommand();
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");


    return true;
}


#include "moc_TaskBooleanParameters.cpp"
