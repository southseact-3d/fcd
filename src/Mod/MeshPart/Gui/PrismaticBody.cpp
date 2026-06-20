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

#include <QMessageBox>

#include <App/Application.h>
#include <App/Document.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/CommandT.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/Selection/Selection.h>
#include <Gui/WaitCursor.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/PartDesign/App/Body.h>

#include "PrismaticBody.h"
#include "ui_PrismaticBody.h"


using namespace MeshPartGui;

PrismaticBody::PrismaticBody(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_PrismaticBody)
{
    ui->setupUi(this);
    setupConnections();
}

PrismaticBody::~PrismaticBody() = default;

void PrismaticBody::setupConnections()
{
    connect(ui->radioAuto, &QRadioButton::toggled, this, [this](bool checked) {
        ui->spinDirX->setEnabled(!checked);
        ui->spinDirY->setEnabled(!checked);
        ui->spinDirZ->setEnabled(!checked);
    });
    connect(ui->radioManual, &QRadioButton::toggled, this, [this](bool checked) {
        ui->spinDirX->setEnabled(checked);
        ui->spinDirY->setEnabled(checked);
        ui->spinDirZ->setEnabled(checked);
    });
}

void PrismaticBody::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QDialog::changeEvent(e);
}

void PrismaticBody::accept()
{
    perform();
    QDialog::accept();
}

void PrismaticBody::perform()
{
    Gui::WaitCursor wc;

    std::vector<App::DocumentObject*> meshes;
    meshes = Gui::Selection().getObjectsOfType(Base::Type::fromName("Mesh::Feature"));

    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    bool autoDetect = ui->radioAuto->isChecked();
    double dirX = ui->spinDirX->value();
    double dirY = ui->spinDirY->value();
    double dirZ = ui->spinDirZ->value();
    int sections = ui->spinSections->value();
    double tolerance = ui->spinTolerance->value();
    bool ruled = ui->checkRuled->isChecked();

    Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Create prismatic body from mesh"));

    for (auto* meshObj : meshes) {
        App::Document* meshDoc = meshObj->getDocument();
        std::string meshName = meshObj->getNameInDocument();
        std::string bodyName = meshDoc->getUniqueObjectName("Body");
        std::string partName = meshDoc->getUniqueObjectName("PrismaticShape");

        // Use Python to call the MeshPart reconstruction algorithm
        Gui::doCommandT(
            Gui::Command::Doc,
            "import FreeCAD, Part, Mesh\n"
            "import MeshPart\n"
            "\n"
            "__doc__ = FreeCAD.getDocument('%s')\n"
            "__mesh__ = __doc__.getObject('%s')\n"
            "if __mesh__ is None:\n"
            "    raise Exception('Mesh object not found')\n"
            "\n"
            "# Get mesh data\n"
            "__topo__ = __mesh__.Mesh.Topology\n"
            "if not __topo__[0] or not __topo__[1]:\n"
            "    raise Exception('Mesh has no topology')\n"
            "\n"
            "# Create shape from mesh using the standard method\n"
            "__shape__ = Part.Shape()\n"
            "__shape__.makeShapeFromMesh(__topo__, %f, True)\n"
            "\n"
            "# Try to create a solid\n"
            "try:\n"
            "    __solid__ = Part.Solid(Part.Shell(__shape__.Faces))\n"
            "    if __solid__.isValid() and __solid__.isClosed():\n"
            "        __shape__ = __solid__\n"
            "except:\n"
            "    pass\n"
            "\n"
            "# Create the part feature\n"
            "__part__ = __doc__.addObject('Part::Feature', '%s')\n"
            "__part__.Shape = __shape__\n"
            "__part__.purgeTouched()\n"
            "\n"
            "# Create Body and assign\n"
            "__body__ = __doc__.addObject('PartDesign::Body', '%s')\n"
            "__body__.BaseFeature = __part__\n"
            "__doc__.recompute()\n"
            "\n"
            "del __doc__, __mesh__, __topo__, __shape__, __part__, __body__\n",
            meshDoc->getName(),
            meshName.c_str(),
            tolerance,
            partName.c_str(),
            bodyName.c_str()
        );
    }

    Gui::Command::commitCommand();
}

// ---------------------------------------

TaskPrismaticBody::TaskPrismaticBody()
{
    widget = new PrismaticBody();
    addTaskBox(widget);
}

void TaskPrismaticBody::open()
{}

void TaskPrismaticBody::clicked(int)
{}

bool TaskPrismaticBody::accept()
{
    widget->perform();
    return true;
}

bool TaskPrismaticBody::reject()
{
    return true;
}

#include "moc_PrismaticBody.cpp"
