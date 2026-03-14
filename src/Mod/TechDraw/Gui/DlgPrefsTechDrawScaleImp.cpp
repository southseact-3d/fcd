#include "DlgPrefsTechDrawScaleImp.h"
#include "ui_DlgPrefsTechDrawScale.h"


using namespace TechDrawGui;

DlgPrefsTechDrawScaleImp::DlgPrefsTechDrawScaleImp( QWidget* parent )
  : PreferencePage( parent )
  , ui(new Ui_DlgPrefsTechDrawScaleImp)
{
    ui->setupUi(this);

    ui->pdsbTemplateMark->setUnit(Base::Unit::Length);
    ui->pdsbTemplateMark->setMinimum(0);

    connect(ui->cbViewScaleType, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DlgPrefsTechDrawScaleImp::onScaleTypeChanged);
}

DlgPrefsTechDrawScaleImp::~DlgPrefsTechDrawScaleImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPrefsTechDrawScaleImp::onScaleTypeChanged(int index)
{
    // disable custom scale if the scale type is not custom

    if (index == 2) // if custom
        ui->pdsbViewScale->setEnabled(true);
    else
        ui->pdsbViewScale->setEnabled(false);
}

void DlgPrefsTechDrawScaleImp::saveSettings()
{
    ui->pdsbPageScale->onSave();
    ui->cbViewScaleType->onSave();
    ui->pdsbViewScale->onSave();
    ui->pdsbVertexScale->onSave();
    ui->pdsbCenterScale->onSave();
    ui->pdsbTemplateMark->onSave();
    ui->pdsbSymbolScale->onSave();
    ui->cbLegacyScale->onSave();
}

void DlgPrefsTechDrawScaleImp::loadSettings()
{
    ui->pdsbPageScale->onRestore();
    ui->cbViewScaleType->onRestore();
    ui->pdsbViewScale->onRestore();
    ui->pdsbVertexScale->onRestore();
    ui->pdsbCenterScale->onRestore();
    double markDefault = 3.0;
    ui->pdsbTemplateMark->setValue(markDefault);
    ui->pdsbTemplateMark->onRestore();
    ui->pdsbSymbolScale->onRestore();
    ui->cbLegacyScale->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgPrefsTechDrawScaleImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include <Mod/TechDraw/Gui/moc_DlgPrefsTechDrawScaleImp.cpp>
