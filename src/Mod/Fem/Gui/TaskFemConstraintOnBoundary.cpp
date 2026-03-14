#include "TaskFemConstraintOnBoundary.h"


using namespace FemGui;
using namespace Gui;

/* TRANSLATOR FemGui::TaskFemConstraintOnBoundary */

TaskFemConstraintOnBoundary::TaskFemConstraintOnBoundary(
    ViewProviderFemConstraint* ConstraintView,
    QWidget* parent,
    const char* pixmapname
)
    : TaskFemConstraint(ConstraintView, parent, pixmapname)
    , selChangeMode(SelectionChangeModes::none)
{
    ConstraintView->highlightReferences(true);

    buttonGroup = new ButtonGroup(this);
    buttonGroup->setExclusive(true);

    connect(
        buttonGroup,
        qOverload<QAbstractButton*, bool>(&QButtonGroup::buttonToggled),
        this,
        &TaskFemConstraintOnBoundary::onButtonToggled
    );
}

TaskFemConstraintOnBoundary::~TaskFemConstraintOnBoundary()
{
    if (!ConstraintView.expired()) {
        ConstraintView->highlightReferences(false);
    }
}
void TaskFemConstraintOnBoundary::onButtonToggled(QAbstractButton* button, bool checked)
{
    auto mode = static_cast<SelectionChangeModes>(buttonGroup->id(button));

    Gui::Selection().clearSelection();

    if (checked) {
        selChangeMode = mode;
        ConstraintView->highlightReferences(true);
    }
    else {
        if (selChangeMode == mode) {
            selChangeMode = SelectionChangeModes::none;
        }
    }
}

void TaskFemConstraintOnBoundary::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        switch (selChangeMode) {
            case SelectionChangeModes::refAdd:
                // TODO: Optimize to just perform actions on the newly selected item. Suggestion
                // from PartDesign: ui->lw_references->addItem(makeRefText(msg.pObjectName,
                // msg.pSubName));
                this->addToSelection();
                break;
            case SelectionChangeModes::refRemove:
                this->removeFromSelection();
                break;
            case SelectionChangeModes::none:
                return;
            default:
                return;
        }
        ConstraintView->highlightReferences(true);
    }
}

#include "moc_TaskFemConstraintOnBoundary.cpp"
