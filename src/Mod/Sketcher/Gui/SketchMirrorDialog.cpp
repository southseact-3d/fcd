#include <QDialog>


#include <Gui/MainWindow.h>

#include "SketchMirrorDialog.h"
#include "ui_SketchMirrorDialog.h"


using namespace SketcherGui;

SketchMirrorDialog::SketchMirrorDialog()
    : QDialog(Gui::getMainWindow())
    , RefGeoid(-1)
    , RefPosid(Sketcher::PointPos::none)
    , ui(new Ui_SketchMirrorDialog)
{
    ui->setupUi(this);
}

SketchMirrorDialog::~SketchMirrorDialog()
{}

void SketchMirrorDialog::accept()
{
    if (ui->XAxisRadioButton->isChecked()) {
        RefGeoid = Sketcher::GeoEnum::HAxis;
        RefPosid = Sketcher::PointPos::none;
    }
    else if (ui->YAxisRadioButton->isChecked()) {
        RefGeoid = Sketcher::GeoEnum::VAxis;
        RefPosid = Sketcher::PointPos::none;
    }
    else if (ui->OriginRadioButton->isChecked()) {
        RefGeoid = Sketcher::GeoEnum::RtPnt;
        RefPosid = Sketcher::PointPos::start;
    }

    QDialog::accept();
}

#include "moc_SketchMirrorDialog.cpp"
