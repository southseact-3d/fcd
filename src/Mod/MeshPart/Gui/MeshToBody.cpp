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

#include "MeshToBody.h"
#include "ui_MeshToBody.h"


using namespace MeshPartGui;

MeshToBody::MeshToBody(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_MeshToBody)
{
    ui->setupUi(this);
    setupConnections();
}

MeshToBody::~MeshToBody() = default;

void MeshToBody::setupConnections()
{
    connect(ui->radioSmart, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->spinTolerance->setValue(0.1);
        }
    });
    connect(ui->radioFast, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->spinTolerance->setValue(1e-6);
        }
    });
}

void MeshToBody::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QDialog::changeEvent(e);
}

void MeshToBody::accept()
{
    perform();
    QDialog::accept();
}

void MeshToBody::perform()
{
    bool smart = ui->radioSmart->isChecked();
    double tolerance = ui->spinTolerance->value();
    bool sewShape = ui->checkSew->isChecked();

    Gui::WaitCursor wc;

    std::vector<App::DocumentObject*> meshes;
    meshes = Gui::Selection().getObjectsOfType(Base::Type::fromName("Mesh::Feature"));

    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Convert mesh to body"));

    for (auto* meshObj : meshes) {
        App::Document* meshDoc = meshObj->getDocument();
        std::string meshName = meshObj->getNameInDocument();
        std::string bodyName = meshDoc->getUniqueObjectName("Body");
        std::string partName = meshDoc->getUniqueObjectName("Shape");

        if (smart) {
            // Smart method: merge coplanar facets for cleaner geometry
            // Uses the Python ArchCommands.getShapeFromMesh approach
            Gui::doCommandT(
                Gui::Command::Doc,
                "import Part, Mesh\n"
                "import MeshPart\n"
                "__mesh__ = FreeCAD.getDocument('%s').getObject('%s')\n"
                "if __mesh__ is None:\n"
                "    raise Exception('Mesh object not found')\n"
                "__topo__ = __mesh__.Mesh.Topology\n"
                "if not __topo__[0] or not __topo__[1]:\n"
                "    raise Exception('Mesh has no topology')\n"
                "__shape__ = Part.Shape()\n"
                "__shape__.makeShapeFromMesh(__topo__, %f, %s)\n"
                "if __shape__.isNull():\n"
                "    raise Exception('Failed to create shape from mesh')\n"
                "del __topo__\n",
                meshDoc->getName(),
                meshName.c_str(),
                tolerance,
                sewShape ? "True" : "False"
            );
        }
        else {
            // Fast method: one face per triangle
            Gui::doCommandT(
                Gui::Command::Doc,
                "import Part, Mesh\n"
                "__mesh__ = FreeCAD.getDocument('%s').getObject('%s')\n"
                "if __mesh__ is None:\n"
                "    raise Exception('Mesh object not found')\n"
                "__topo__ = __mesh__.Mesh.Topology\n"
                "if not __topo__[0] or not __topo__[1]:\n"
                "    raise Exception('Mesh has no topology')\n"
                "__shape__ = Part.Shape()\n"
                "__shape__.makeShapeFromMesh(__topo__, %f, %s)\n"
                "if __shape__.isNull():\n"
                "    raise Exception('Failed to create shape from mesh')\n"
                "del __topo__\n",
                meshDoc->getName(),
                meshName.c_str(),
                tolerance,
                sewShape ? "True" : "False"
            );
        }

        // Convert to solid if possible
        Gui::doCommandT(
            Gui::Command::Doc,
            "__doc__ = FreeCAD.getDocument('%s')\n"
            "__part__ = __doc__.addObject('Part::Feature', '%s')\n"
            "__part__.Shape = __shape__\n"
            "__part__.purgeTouched()\n"
            // Try to create a solid from the shape
            "try:\n"
            "    __solid__ = Part.Solid(Part.Shell(__part__.Shape.Faces))\n"
            "    if __solid__.isValid() and __solid__.isClosed():\n"
            "        __part__.Shape = __solid__\n"
            "except:\n"
            "    pass  # Keep as-is if solid creation fails\n"
            // Create Body and assign the shape
            "__body__ = __doc__.addObject('PartDesign::Body', '%s')\n"
            "__body__.BaseFeature = __part__\n"
            "__doc__.recompute()\n"
            "del __doc__, __part__, __body__, __shape__\n",
            meshDoc->getName(),
            partName.c_str(),
            bodyName.c_str()
        );
    }

    Gui::Command::commitCommand();
}

// ---------------------------------------

TaskMeshToBody::TaskMeshToBody()
{
    widget = new MeshToBody();
    addTaskBox(widget);
}

void TaskMeshToBody::open()
{}

void TaskMeshToBody::clicked(int)
{}

bool TaskMeshToBody::accept()
{
    widget->perform();
    return true;
}

bool TaskMeshToBody::reject()
{
    return true;
}

#include "moc_MeshToBody.cpp"
