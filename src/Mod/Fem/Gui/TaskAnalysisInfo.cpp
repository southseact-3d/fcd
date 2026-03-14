#include <Gui/BitmapFactory.h>

#include "TaskAnalysisInfo.h"
#include "ui_TaskAnalysisInfo.h"


using namespace FemGui;
using namespace Gui;

TaskAnalysisInfo::TaskAnalysisInfo(Fem::FemAnalysis* pcObject, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("FEM_Analysis"), tr("Nodes set"), true, parent)
    , pcObject(pcObject)
    , ui(new Ui_TaskAnalysisInfo)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
}

TaskAnalysisInfo::~TaskAnalysisInfo() = default;

#include "moc_TaskAnalysisInfo.cpp"
