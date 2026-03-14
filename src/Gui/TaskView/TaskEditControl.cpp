#include <QDialogButtonBox>
#include <QHBoxLayout>

#include "TaskEditControl.h"


using namespace Gui::TaskView;

TaskEditControl::TaskEditControl(QWidget* parent)
    : TaskWidget(parent)
{
    hboxLayout = new QHBoxLayout(this);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(true);

    hboxLayout->addWidget(buttonBox);
}

TaskEditControl::~TaskEditControl() = default;

QDialogButtonBox* TaskEditControl::standardButtons() const
{
    return buttonBox;
}

#include "moc_TaskEditControl.cpp"
