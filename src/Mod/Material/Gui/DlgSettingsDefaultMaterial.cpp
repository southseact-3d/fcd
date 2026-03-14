#include <Mod/Material/App/ModelUuids.h>

#include "DlgSettingsDefaultMaterial.h"
#include "ui_DlgSettingsDefaultMaterial.h"


using namespace MatGui;

DlgSettingsDefaultMaterial::DlgSettingsDefaultMaterial(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsDefaultMaterial)
{
    ui->setupUi(this);

    ui->widgetMaterial->setParamGrpPath("Mod/Material");
    ui->widgetMaterial->setEntryName("DefaultMaterial");

    setupFilters();
}

void DlgSettingsDefaultMaterial::setupFilters()
{
    // Create a filter to only include current format materials
    // that contain at a minimum the Density model
    auto filterList = std::make_shared<std::list<std::shared_ptr<Materials::MaterialFilter>>>();

    auto filter = std::make_shared<Materials::MaterialFilter>();
    filter->setName(tr("Physical"));
    filter->addRequiredComplete(Materials::ModelUUIDs::ModelUUID_Mechanical_Density);
    filterList->push_back(filter);

    ui->widgetMaterial->setIncludeFavorites(false);
    ui->widgetMaterial->setIncludeRecent(false);
    ui->widgetMaterial->setIncludeEmptyFolders(false);
    ui->widgetMaterial->setIncludeLegacy(false);

    ui->widgetMaterial->setFilter(filterList);
}

void DlgSettingsDefaultMaterial::saveSettings()
{
    ui->widgetMaterial->onSave();
}

void DlgSettingsDefaultMaterial::loadSettings()
{
    ui->widgetMaterial->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsDefaultMaterial::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsDefaultMaterial.cpp"
