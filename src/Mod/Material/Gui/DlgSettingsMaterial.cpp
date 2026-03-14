#include "DlgSettingsMaterial.h"
#include "ui_DlgSettingsMaterial.h"


using namespace MatGui;

DlgSettingsMaterial::DlgSettingsMaterial(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsMaterial)
{
    ui->setupUi(this);
}

void DlgSettingsMaterial::saveSettings()
{
    ui->cb_use_built_in_materials->onSave();
    ui->cb_use_mat_from_workbenches->onSave();
    ui->cb_use_mat_from_config_dir->onSave();
    ui->cb_use_mat_from_custom_dir->onSave();
    ui->fc_custom_mat_dir->onSave();
    ui->cb_delete_duplicates->onSave();
    ui->cb_sort_by_resources->onSave();
    ui->cb_show_favorites->onSave();
    ui->cb_show_recent->onSave();
    ui->cb_show_empty_libraries->onSave();
    ui->cb_show_empty_folders->onSave();
    ui->cb_show_legacy->onSave();
    ui->cb_show_favorites_editor->onSave();
    ui->cb_show_recent_editor->onSave();
    ui->cb_show_empty_libraries_editor->onSave();
    ui->cb_show_empty_folders_editor->onSave();
    ui->cb_show_legacy_editor->onSave();

    // Temporary for testing
    // ui->cb_legacy_editor->onSave();
}

void DlgSettingsMaterial::loadSettings()
{
    ui->cb_use_built_in_materials->onRestore();
    ui->cb_use_mat_from_workbenches->onRestore();
    ui->cb_use_mat_from_config_dir->onRestore();
    ui->cb_use_mat_from_custom_dir->onRestore();
    ui->fc_custom_mat_dir->onRestore();
    ui->cb_delete_duplicates->onRestore();
    ui->cb_sort_by_resources->onRestore();
    ui->cb_show_favorites->onRestore();
    ui->cb_show_recent->onRestore();
    ui->cb_show_empty_libraries->onRestore();
    ui->cb_show_empty_folders->onRestore();
    ui->cb_show_legacy->onRestore();
    ui->cb_show_favorites_editor->onRestore();
    ui->cb_show_recent_editor->onRestore();
    ui->cb_show_empty_libraries_editor->onRestore();
    ui->cb_show_empty_folders_editor->onRestore();
    ui->cb_show_legacy_editor->onRestore();

    // Temporary for testing
    // ui->cb_legacy_editor->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsMaterial::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsMaterial.cpp"
