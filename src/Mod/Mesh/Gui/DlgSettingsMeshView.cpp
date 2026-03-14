#include <Gui/Window.h>

#include "DlgSettingsMeshView.h"
#include "ui_DlgSettingsMeshView.h"


using namespace MeshGui;

/**
 *  Constructs a DlgSettingsMeshView which is a child of 'parent'.
 */
DlgSettingsMeshView::DlgSettingsMeshView(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsMeshView)
{
    ui->setupUi(this);
    ui->labelBackfaceColor->hide();
    ui->buttonBackfaceColor->hide();
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsMeshView::~DlgSettingsMeshView() = default;

void DlgSettingsMeshView::saveSettings()
{
    ui->checkboxRendering->onSave();
    ui->checkboxBoundbox->onSave();
    ui->buttonMeshColor->onSave();
    ui->buttonLineColor->onSave();
    ui->buttonBackfaceColor->onSave();
    ui->spinMeshTransparency->onSave();
    ui->spinLineTransparency->onSave();
    ui->checkboxNormal->onSave();
    ui->spinboxAngle->onSave();
}

void DlgSettingsMeshView::loadSettings()
{
    Base::Reference<ParameterGrp> hGrp = Gui::WindowParameter::getDefaultParameter();
    hGrp = hGrp->GetGroup("View");
    if (!hGrp->GetBool("EnablePreselection", true) && !hGrp->GetBool("EnableSelection", true)) {
        ui->checkboxBoundbox->setDisabled(true);
    }
    ui->checkboxRendering->onRestore();
    ui->checkboxBoundbox->onRestore();
    ui->buttonMeshColor->onRestore();
    ui->buttonLineColor->onRestore();
    ui->buttonBackfaceColor->onRestore();
    ui->spinMeshTransparency->onRestore();
    ui->spinLineTransparency->onRestore();
    ui->checkboxNormal->onRestore();
    ui->spinboxAngle->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsMeshView::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        PreferencePage::changeEvent(e);
    }
}

#include "moc_DlgSettingsMeshView.cpp"
