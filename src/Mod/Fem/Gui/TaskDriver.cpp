#include <Gui/BitmapFactory.h>

#include "TaskDriver.h"
#include "ui_TaskDriver.h"


using namespace FemGui;
using namespace Gui;

TaskDriver::TaskDriver(Fem::FemAnalysis* pcObject, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("FEM_CreateNodesSet"), tr("Nodes set"), true, parent)
    , pcObject(pcObject)
    , ui(new Ui_TaskDriver)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
}

TaskDriver::~TaskDriver() = default;

#include "moc_TaskDriver.cpp"
