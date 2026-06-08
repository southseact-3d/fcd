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

#include <limits>

#include <QMessageBox>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/CommandT.h>
#include <Gui/Document.h>
#include <Gui/Selection/Selection.h>
#include <Gui/Selection/SelectionFilter.h>
#include <Gui/Selection/SelectionObject.h>
#include <Gui/ViewProvider.h>
#include <Mod/Part/App/FeatureFaceOffset.h>

#include "TaskFaceOffset.h"
#include "ui_TaskFaceOffset.h"


using namespace PartGui;

class FaceOffsetWidget::Private
{
public:
    Ui_TaskFaceOffset ui {};
    QString text;
    std::string selection;
    Part::FaceOffset* faceOffset {nullptr};

    class FaceSelection: public Gui::SelectionFilterGate
    {
        const App::DocumentObject* object;

    public:
        explicit FaceSelection(const App::DocumentObject* obj)
            : Gui::SelectionFilterGate(nullPointer())
            , object(obj)
        {}
        bool allow(App::Document* /*pDoc*/, App::DocumentObject* pObj, const char* sSubName) override
        {
            if (pObj != this->object) {
                return false;
            }
            if (Base::Tools::isNullOrEmpty(sSubName)) {
                return false;
            }
            std::string element(sSubName);
            return element.substr(0, 4) == "Face";
        }
    };
};

/* TRANSLATOR PartGui::FaceOffsetWidget */

FaceOffsetWidget::FaceOffsetWidget(Part::FaceOffset* faceOffset, QWidget* parent)
    : d(new Private())
{
    Q_UNUSED(parent);
    Gui::Command::runCommand(Gui::Command::App, "from FreeCAD import Base");
    Gui::Command::runCommand(Gui::Command::App, "import Part");

    d->faceOffset = faceOffset;
    d->ui.setupUi(this);
    setupConnections();

    QSignalBlocker blockOffset(d->ui.spinOffset);
    d->ui.spinOffset->setRange(-std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    d->ui.spinOffset->setSingleStep(0.1);
    d->ui.spinOffset->setValue(d->faceOffset->Value.getValue());

    int join = d->faceOffset->Join.getValue();
    d->ui.joinType->setCurrentIndex(join);

    QSignalBlocker blockIntSct(d->ui.intersection);
    bool intsct = d->faceOffset->Intersection.getValue();
    d->ui.intersection->setChecked(intsct);

    d->ui.spinOffset->bind(d->faceOffset->Value);
}

FaceOffsetWidget::~FaceOffsetWidget()
{
    delete d;
    Gui::Selection().rmvSelectionGate();
}

void FaceOffsetWidget::setupConnections()
{
    // clang-format off
    connect(d->ui.spinOffset, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &FaceOffsetWidget::onSpinOffsetValueChanged);
    connect(d->ui.joinType, qOverload<int>(&QComboBox::activated),
            this, &FaceOffsetWidget::onJoinTypeActivated);
    connect(d->ui.intersection, &QCheckBox::toggled,
            this, &FaceOffsetWidget::onIntersectionToggled);
    connect(d->ui.facesButton, &QPushButton::toggled,
            this, &FaceOffsetWidget::onFacesButtonToggled);
    connect(d->ui.updateView, &QCheckBox::toggled,
            this, &FaceOffsetWidget::onUpdateViewToggled);
    // clang-format on
}

Part::FaceOffset* FaceOffsetWidget::getObject() const
{
    return d->faceOffset;
}

void FaceOffsetWidget::onSpinOffsetValueChanged(double val)
{
    d->faceOffset->Value.setValue(val);
    if (d->ui.updateView->isChecked()) {
        d->faceOffset->getDocument()->recomputeFeature(d->faceOffset);
    }
}

void FaceOffsetWidget::onJoinTypeActivated(int val)
{
    d->faceOffset->Join.setValue((long)val);
    if (d->ui.updateView->isChecked()) {
        d->faceOffset->getDocument()->recomputeFeature(d->faceOffset);
    }
}

void FaceOffsetWidget::onIntersectionToggled(bool on)
{
    d->faceOffset->Intersection.setValue(on);
    if (d->ui.updateView->isChecked()) {
        d->faceOffset->getDocument()->recomputeFeature(d->faceOffset);
    }
}

void FaceOffsetWidget::onFacesButtonToggled(bool on)
{
    if (on) {
        QList<QWidget*> c = this->findChildren<QWidget*>();
        for (auto it : c) {
            it->setEnabled(false);
        }
        d->ui.facesButton->setEnabled(true);
        d->ui.labelFaces->setText(tr("Select faces of the source object and press 'Done'"));
        d->ui.labelFaces->setEnabled(true);
        d->text = d->ui.facesButton->text();
        d->ui.facesButton->setText(tr("Done"));

        Gui::Application::Instance->showViewProvider(d->faceOffset->Source.getValue());
        Gui::Application::Instance->hideViewProvider(d->faceOffset);
        Gui::Selection().clearSelection();
        Gui::Selection().addSelectionGate(new Private::FaceSelection(d->faceOffset->Source.getValue()));
    }
    else {
        QList<QWidget*> c = this->findChildren<QWidget*>();
        for (auto it : c) {
            it->setEnabled(true);
        }
        d->ui.facesButton->setText(d->text);
        d->ui.labelFaces->clear();

        d->selection = Gui::Command::getPythonTuple(
            d->faceOffset->Source.getValue()->getNameInDocument(),
            d->faceOffset->Source.getSubValues()
        );
        std::vector<Gui::SelectionObject> sel = Gui::Selection().getSelectionEx();
        for (auto& it : sel) {
            if (it.getObject() == d->faceOffset->Source.getValue()) {
                d->faceOffset->Source.setValue(it.getObject(), it.getSubNames());
                d->selection = it.getAsPropertyLinkSubString();
                break;
            }
        }

        Gui::Selection().rmvSelectionGate();
        Gui::Application::Instance->showViewProvider(d->faceOffset);
        Gui::Application::Instance->hideViewProvider(d->faceOffset->Source.getValue());
        if (d->ui.updateView->isChecked()) {
            d->faceOffset->getDocument()->recomputeFeature(d->faceOffset);
        }
    }
}

void FaceOffsetWidget::onUpdateViewToggled(bool on)
{
    if (on) {
        d->faceOffset->getDocument()->recomputeFeature(d->faceOffset);
    }
}

bool FaceOffsetWidget::accept()
{
    if (d->ui.facesButton->isChecked()) {
        return false;
    }

    try {
        if (!d->selection.empty()) {
            Gui::cmdAppObjectArgs(d->faceOffset, "Source = %s", d->selection.c_str());
        }
        Gui::cmdAppObjectArgs(d->faceOffset, "Value = %f", d->ui.spinOffset->value().getValue());
        Gui::cmdAppObjectArgs(d->faceOffset, "Join = %d", d->ui.joinType->currentIndex());
        Gui::cmdAppObjectArgs(
            d->faceOffset,
            "Intersection = %s",
            d->ui.intersection->isChecked() ? "True" : "False"
        );

        Gui::Command::doCommand(Gui::Command::Doc, "App.ActiveDocument.recompute()");
        if (!d->faceOffset->isValid()) {
            throw Base::CADKernelError(d->faceOffset->getStatusString());
        }
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
        Gui::Command::commitCommand();
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(
            this,
            tr("Input error"),
            QCoreApplication::translate("Exception", e.what())
        );
        return false;
    }

    return true;
}

bool FaceOffsetWidget::reject()
{
    if (d->ui.facesButton->isChecked()) {
        return false;
    }

    // save this and check if the object is still there after the
    // transaction is aborted
    std::string objname = d->faceOffset->getNameInDocument();
    App::DocumentObject* source = d->faceOffset->Source.getValue();

    // roll back the done things
    Gui::Command::abortCommand();
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
    Gui::Command::updateActive();

    // FaceOffset object was deleted
    if (source && !source->getDocument()->getObject(objname.c_str())) {
        Gui::Application::Instance->getViewProvider(source)->show();
    }

    return true;
}

void FaceOffsetWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        d->ui.retranslateUi(this);
    }
}


/* TRANSLATOR PartGui::TaskFaceOffset */

TaskFaceOffset::TaskFaceOffset(Part::FaceOffset* offset)
{
    widget = new FaceOffsetWidget(offset);
    widget->setWindowTitle(FaceOffsetWidget::tr("Face Offset"));
    addTaskBox(Gui::BitmapFactory().pixmap("Part_FaceOffset"), widget);
}

Part::FaceOffset* TaskFaceOffset::getObject() const
{
    return widget->getObject();
}

void TaskFaceOffset::open()
{}

void TaskFaceOffset::clicked(int)
{}

bool TaskFaceOffset::accept()
{
    return widget->accept();
}

bool TaskFaceOffset::reject()
{
    return widget->reject();
}

#include "moc_TaskFaceOffset.cpp"
