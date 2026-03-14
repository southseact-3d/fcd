#include "DlgPrefsMeasureAppearanceImp.h"
#include "ui_DlgPrefsMeasureAppearanceImp.h"

using namespace MeasureGui;

DlgPrefsMeasureAppearanceImp::DlgPrefsMeasureAppearanceImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgPrefsMeasureAppearanceImp)
{
    ui->setupUi(this);
}

DlgPrefsMeasureAppearanceImp::~DlgPrefsMeasureAppearanceImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPrefsMeasureAppearanceImp::saveSettings()
{
    ui->sbFontSize->onSave();
    ui->cbText->onSave();
    ui->cbLine->onSave();
    ui->cbBackground->onSave();
}

void DlgPrefsMeasureAppearanceImp::loadSettings()
{
    ui->sbFontSize->onRestore();
    ui->cbText->onRestore();
    ui->cbBackground->onRestore();
    ui->cbLine->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgPrefsMeasureAppearanceImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include <Mod/Measure/Gui/moc_DlgPrefsMeasureAppearanceImp.cpp>
