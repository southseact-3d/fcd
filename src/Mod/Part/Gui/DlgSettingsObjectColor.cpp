#include "DlgSettingsObjectColor.h"
#include "ui_DlgSettingsObjectColor.h"


using namespace PartGui;

/* TRANSLATOR PartGui::DlgSettingsObjectColor */

/**
 *  Constructs a DlgSettingsObjectColor which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DlgSettingsObjectColor::DlgSettingsObjectColor(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsObjectColor)
{
    ui->setupUi(this);
    ui->DefaultShapeColor->setDisabled(ui->checkRandomColor->isChecked());
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsObjectColor::~DlgSettingsObjectColor() = default;

void DlgSettingsObjectColor::saveSettings()
{
    // Part
    ui->DefaultShapeColor->onSave();
    ui->DefaultAmbientColor->onSave();
    ui->DefaultEmissiveColor->onSave();
    ui->DefaultSpecularColor->onSave();
    ui->checkRandomColor->onSave();
    ui->DefaultShapeTransparency->onSave();
    ui->DefaultShapeShininess->onSave();
    ui->DefaultShapeLineColor->onSave();
    ui->DefaultShapeLineWidth->onSave();
    ui->DefaultShapeVertexColor->onSave();
    ui->DefaultShapeVertexSize->onSave();
    ui->BoundingBoxColor->onSave();
    ui->BoundingBoxFontSize->onSave();
    ui->twosideRendering->onSave();
    // Annotations
    ui->AnnotationTextColor->onSave();
}

void DlgSettingsObjectColor::loadSettings()
{
    // Part
    ui->DefaultShapeColor->onRestore();
    ui->DefaultAmbientColor->onRestore();
    ui->DefaultEmissiveColor->onRestore();
    ui->DefaultSpecularColor->onRestore();
    ui->checkRandomColor->onRestore();
    ui->DefaultShapeTransparency->onRestore();
    ui->DefaultShapeShininess->onRestore();
    ui->DefaultShapeLineColor->onRestore();
    ui->DefaultShapeLineWidth->onRestore();
    ui->DefaultShapeVertexColor->onRestore();
    ui->DefaultShapeVertexSize->onRestore();
    ui->BoundingBoxColor->onRestore();
    ui->BoundingBoxFontSize->onRestore();
    ui->twosideRendering->onRestore();
    // Annotations
    ui->AnnotationTextColor->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsObjectColor::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsObjectColor.cpp"
