#include "DlgPrefsTechDrawGeneralImp.h"
#include "ui_DlgPrefsTechDrawGeneral.h"
#include "PreferencesGui.h"
#include "DrawGuiUtil.h"


using namespace TechDrawGui;
using namespace TechDraw;

DlgPrefsTechDrawGeneralImp::DlgPrefsTechDrawGeneralImp( QWidget* parent )
  : PreferencePage( parent )
  , ui(new Ui_DlgPrefsTechDrawGeneralImp)
{
    ui->setupUi(this);
    ui->plsb_LabelSize->setUnit(Base::Unit::Length);
    ui->plsb_LabelSize->setMinimum(0);

    ui->psb_GridSpacing->setUnit(Base::Unit::Length);
    ui->psb_GridSpacing->setMinimum(0);

    ui->pfc_DefDir->setMode(Gui::FileChooser::Mode::Directory);
    ui->pfc_Welding->setMode(Gui::FileChooser::Mode::Directory);
    ui->fcSymbolDir->setMode(Gui::FileChooser::Mode::Directory);
}

DlgPrefsTechDrawGeneralImp::~DlgPrefsTechDrawGeneralImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPrefsTechDrawGeneralImp::saveSettings()
{
    ui->cb_Global->onSave();
    ui->cb_Override->onSave();
    ui->cb_PageUpdate->onSave();
    ui->cb_AutoDist->onSave();

    ui->pfb_LabelFont->onSave();
    ui->plsb_LabelSize->onSave();

    ui->cbProjAngle->onSave();
    ui->cbSectionLineStd->onSave();

    ui->pfc_DefTemp->onSave();
    ui->pfc_DefDir->onSave();
    ui->pfc_HatchFile->onSave();
    ui->pfc_LineGroup->onSave();
    ui->pfc_Welding->onSave();
    ui->pfc_FilePattern->onSave();
    ui->le_NamePattern->onSave();
    ui->fcSymbolDir->onSave();

    ui->cb_ShowGrid->onSave();
    ui->psb_GridSpacing->onSave();

    ui->cbMultiSelection->onSave();

    ui->cb_viewFramesVisibility->onSave();
    ui->cb_useCameraDirection->onSave();
    ui->cb_SnapViews->onSave();
    ui->psb_SnapFactor->onSave();
    ui->cb_SnapHighlights->onSave();
    ui->psb_HighlightSnapFactor->onSave();
}

void DlgPrefsTechDrawGeneralImp::loadSettings()
{
    ui->cb_Global->onRestore();
    ui->cb_Override->onRestore();
    ui->cb_PageUpdate->onRestore();
    ui->cb_AutoDist->onRestore();

    double labelDefault = Preferences::labelFontSizeMM();
    ui->plsb_LabelSize->setValue(labelDefault);
    QFont prefFont(Preferences::labelFontQString());
    ui->pfb_LabelFont->setCurrentFont(prefFont);

    ui->pfb_LabelFont->onRestore();
    ui->plsb_LabelSize->onRestore();

    ui->cbProjAngle->onRestore();
    ui->cbSectionLineStd->onRestore();

    ui->pfc_DefTemp->onRestore();
    ui->pfc_DefDir->onRestore();
    ui->pfc_HatchFile->onRestore();
    ui->pfc_LineGroup->onRestore();
    ui->pfc_Welding->onRestore();
    ui->pfc_FilePattern->onRestore();
    ui->le_NamePattern->onRestore();
    ui->fcSymbolDir->onRestore();


    bool gridDefault = PreferencesGui::showGrid();
    ui->cb_ShowGrid->setChecked(gridDefault);
    ui->cb_ShowGrid->onRestore();

    double spacingDefault = PreferencesGui::gridSpacing();
    ui->psb_GridSpacing->setValue(spacingDefault);
    ui->psb_GridSpacing->onRestore();

    bool multiSelectionDefault = PreferencesGui::multiSelection();
    ui->cbMultiSelection->setChecked(multiSelectionDefault);
    ui->cbMultiSelection->onRestore();

    ui->cb_viewFramesVisibility->onRestore();
    ui->cb_useCameraDirection->onRestore();

    ui->cb_SnapViews->onRestore();
    ui->psb_SnapFactor->onRestore();

    ui->cb_SnapHighlights->onRestore();
    ui->psb_HighlightSnapFactor->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgPrefsTechDrawGeneralImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include <Mod/TechDraw/Gui/moc_DlgPrefsTechDrawGeneralImp.cpp>
