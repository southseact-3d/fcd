#include "DlgPrefsTechDrawHLRImp.h"
#include "ui_DlgPrefsTechDrawHLR.h"


using namespace TechDrawGui;

DlgPrefsTechDrawHLRImp::DlgPrefsTechDrawHLRImp( QWidget* parent )
  : PreferencePage( parent )
  , ui(new Ui_DlgPrefsTechDrawHLRImp)
{
    ui->setupUi(this);
}

DlgPrefsTechDrawHLRImp::~DlgPrefsTechDrawHLRImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPrefsTechDrawHLRImp::saveSettings()
{
    ui->pcbSeamViz->onSave();
    ui->pcbSmoothViz->onSave();
    ui->pcbHardViz->onSave();
    ui->pcbPolygon->onSave();
    ui->pcbIsoViz->onSave();
    ui->pcbSmoothHid->onSave();
    ui->pcbSeamHid->onSave();
    ui->pcbIsoHid->onSave();
    ui->psbIsoCount->onSave();
    ui->pcbHardHid->onSave();
}

void DlgPrefsTechDrawHLRImp::loadSettings()
{
    // set defaults for HLR
    ui->pcbSeamViz->onRestore();

    ui->pcbSmoothViz->onRestore();
    ui->pcbHardViz->onRestore();
    ui->pcbPolygon->onRestore();
    ui->pcbIsoViz->onRestore();
    ui->pcbSmoothHid->onRestore();
    ui->pcbSeamHid->onRestore();
    ui->pcbIsoHid->onRestore();
    ui->psbIsoCount->onRestore();
    ui->pcbHardHid->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgPrefsTechDrawHLRImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include <Mod/TechDraw/Gui/moc_DlgPrefsTechDrawHLRImp.cpp>
