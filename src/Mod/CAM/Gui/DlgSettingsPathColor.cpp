#include "DlgSettingsPathColor.h"
#include "ui_DlgSettingsPathColor.h"


using namespace PathGui;

/* TRANSLATOR PathGui::DlgSettingsPathColor */

/**
 *  Constructs a DlgSettingsObjectColor which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DlgSettingsPathColor::DlgSettingsPathColor(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsPathColor)
{
    ui->setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsPathColor::~DlgSettingsPathColor()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgSettingsPathColor::saveSettings()
{
    // Part
    ui->DefaultNormalPathColor->onSave();
    ui->DefaultRapidPathColor->onSave();
    ui->DefaultPathLineWidth->onSave();
    ui->DefaultPathMarkerColor->onSave();
    ui->DefaultProbePathColor->onSave();
    ui->DefaultHighlightPathColor->onSave();
    ui->DefaultBBoxSelectionColor->onSave();
    ui->DefaultBBoxNormalColor->onSave();
    ui->DefaultSelectionStyle->onSave();
    ui->DefaultTaskPanelLayout->onSave();
    ui->HideFirstRapid->onSave();
}

void DlgSettingsPathColor::loadSettings()
{
    // Part
    ui->DefaultNormalPathColor->onRestore();
    ui->DefaultRapidPathColor->onRestore();
    ui->DefaultPathLineWidth->onRestore();
    ui->DefaultPathMarkerColor->onRestore();
    ui->DefaultProbePathColor->onRestore();
    ui->DefaultHighlightPathColor->onRestore();
    ui->DefaultBBoxSelectionColor->onRestore();
    ui->DefaultBBoxNormalColor->onRestore();
    ui->DefaultSelectionStyle->onRestore();
    ui->DefaultTaskPanelLayout->onRestore();
    ui->HideFirstRapid->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsPathColor::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsPathColor.cpp"
