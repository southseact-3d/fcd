#include <QString>


#include <Gui/BitmapFactory.h>
#include <Gui/Placement.h>
#include <Gui/Selection/Selection.h>

#include "TaskTrajectoryDressUpParameter.h"
#include "ui_TaskTrajectoryDressUpParameter.h"


using namespace RobotGui;
using namespace Gui;

TaskTrajectoryDressUpParameter::TaskTrajectoryDressUpParameter(
    Robot::TrajectoryDressUpObject* obj,
    QWidget* parent
)
    : TaskBox(Gui::BitmapFactory().pixmap("Robot_TrajectoryDressUp"), tr("Dress Up Parameter"), true, parent)
    , pcObject(obj)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskTrajectoryDressUpParameter();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    // pump the actual values in the Gui
    ui->doubleSpinBoxSpeed->setValue(pcObject->Speed.getValue() / 1000.0);
    ui->doubleSpinBoxAccel->setValue(pcObject->Acceleration.getValue() / 1000.0);
    ui->checkBoxUseSpeed->setChecked(pcObject->UseSpeed.getValue());
    ui->checkBoxUseAccel->setChecked(pcObject->UseAcceleration.getValue());
    ui->comboBoxCont->setCurrentIndex(pcObject->ContType.getValue());
    ui->comboBoxOrientation->setCurrentIndex(pcObject->AddType.getValue());

    PosAdd = pcObject->PosAdd.getValue();
    viewPlacement();

    QObject::connect(
        ui->toolButtonChoosePlacement,
        &QToolButton::clicked,
        this,
        &TaskTrajectoryDressUpParameter::createPlacementDlg
    );
}


TaskTrajectoryDressUpParameter::~TaskTrajectoryDressUpParameter()
{
    delete ui;
}

void TaskTrajectoryDressUpParameter::writeValues()
{
    pcObject->Speed.setValue(ui->doubleSpinBoxSpeed->value() * 1000.0);
    pcObject->Acceleration.setValue(ui->doubleSpinBoxAccel->value() * 1000.0);
    pcObject->UseSpeed.setValue(ui->checkBoxUseSpeed->isChecked());
    pcObject->UseAcceleration.setValue(ui->checkBoxUseAccel->isChecked());
    pcObject->ContType.setValue(ui->comboBoxCont->currentIndex());
    pcObject->AddType.setValue(ui->comboBoxOrientation->currentIndex());
    pcObject->PosAdd.setValue(PosAdd);
}

void TaskTrajectoryDressUpParameter::createPlacementDlg()
{
    Gui::Dialog::Placement plc;
    plc.setSelection(Gui::Selection().getSelectionEx());
    plc.setPlacement(PosAdd);
    if (plc.exec() == QDialog::Accepted) {
        PosAdd = plc.getPlacement();
        viewPlacement();
    }
}


void TaskTrajectoryDressUpParameter::viewPlacement()
{
    double A, B, C;
    Base::Vector3d pos = PosAdd.getPosition();
    PosAdd.getRotation().getYawPitchRoll(A, B, C);
    QString val = QStringLiteral("(%1,%2,%3),(%4,%5,%6)\n")
                      .arg(pos.x, 0, 'g', 6)
                      .arg(pos.y, 0, 'g', 6)
                      .arg(pos.z, 0, 'g', 6)
                      .arg(A, 0, 'g', 6)
                      .arg(B, 0, 'g', 6)
                      .arg(C, 0, 'g', 6);

    ui->lineEditPlacement->setText(val);
}

#include "moc_TaskTrajectoryDressUpParameter.cpp"
