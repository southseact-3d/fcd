#include "DlgPointsReadImp.h"
#include "ui_DlgPointsRead.h"


using namespace PointsGui;

DlgPointsReadImp::DlgPointsReadImp(const char* FileName, QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_DlgPointsRead)
    , _FileName(FileName)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgPointsReadImp::~DlgPointsReadImp() = default;


#include "moc_DlgPointsReadImp.cpp"
