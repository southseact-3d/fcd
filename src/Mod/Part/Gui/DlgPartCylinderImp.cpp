#include "DlgPartCylinderImp.h"
#include "ui_DlgPartCylinder.h"


using namespace PartGui;

DlgPartCylinderImp::DlgPartCylinderImp(QWidget* parent, Qt::WindowFlags fl)
    : Gui::LocationDialogUiImp(new Ui_DlgPartCylinder, parent, fl)
{}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgPartCylinderImp::~DlgPartCylinderImp() = default;

Ui_DlgPartCylinderPtr DlgPartCylinderImp::getUi() const
{
    return boost::any_cast<Ui_DlgPartCylinderPtr>(ui->get());
}

double DlgPartCylinderImp::getRadius() const
{
    return getUi()->radius->value().getValue();
}

double DlgPartCylinderImp::getLength() const
{
    return getUi()->length->value().getValue();
}

#include "moc_DlgPartCylinderImp.cpp"
