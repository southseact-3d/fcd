#include <QButtonGroup>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>
#include <Interface_Static.hxx>


#include <Mod/Part/App/Interface.h>
#include <Mod/Part/App/FuzzyHelper.h>
#include <Mod/Part/App/IGES/ImportExportSettings.h>
#include <Mod/Part/App/OCAF/ImportExportSettings.h>
#include <Mod/Part/App/STEP/ImportExportSettings.h>

#include "DlgSettingsGeneral.h"
#include "ui_DlgSettingsGeneral.h"
#include "ui_DlgImportExportIges.h"
#include "DlgExportStep.h"
#include "DlgImportStep.h"


using namespace PartGui;

DlgSettingsGeneral::DlgSettingsGeneral(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsGeneral)
{
    ui->setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsGeneral::~DlgSettingsGeneral() = default;

void DlgSettingsGeneral::saveSettings()
{
    ui->checkBooleanCheck->onSave();
    ui->checkBooleanRefine->onSave();
    ui->checkSketchBaseRefine->onSave();
    ui->checkObjectNaming->onSave();
    ui->checkAllowCompoundBody->onSave();
    ui->enableGizmos->onSave();
    ui->delayedGizmoUpdate->onSave();
    ui->comboDefaultProfileTypeForHole->onSave();
    ui->checkShowFinalPreview->onSave();
    ui->checkShowTransparentPreview->onSave();
    ui->checkShowProfilePreview->onSave();
    ui->checkSwitchToTask->onSave();
}

void DlgSettingsGeneral::loadSettings()
{
    ui->checkBooleanCheck->onRestore();
    ui->checkBooleanRefine->onRestore();
    ui->checkSketchBaseRefine->onRestore();
    ui->checkObjectNaming->onRestore();
    ui->checkAllowCompoundBody->onRestore();
    ui->enableGizmos->onRestore();
    ui->delayedGizmoUpdate->onRestore();
    ui->comboDefaultProfileTypeForHole->onRestore();
    ui->checkShowFinalPreview->onRestore();
    ui->checkShowTransparentPreview->onRestore();
    ui->checkShowProfilePreview->onRestore();
    ui->checkSwitchToTask->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsGeneral::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

// ----------------------------------------------------------------------------

DlgImportExportIges::DlgImportExportIges(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgImportExportIges)
{
    ui->setupUi(this);
    ui->lineEditProduct->setReadOnly(true);

    bg = new QButtonGroup(this);
    bg->addButton(ui->radioButtonBRepOff, 0);
    bg->addButton(ui->radioButtonBRepOn, 1);

    QRegularExpression rx;
    rx.setPattern(QStringLiteral("[\\x00-\\x7F]+"));
    QRegularExpressionValidator* companyValidator = new QRegularExpressionValidator(
        ui->lineEditCompany
    );
    companyValidator->setRegularExpression(rx);
    ui->lineEditCompany->setValidator(companyValidator);
    QRegularExpressionValidator* authorValidator = new QRegularExpressionValidator(ui->lineEditAuthor);
    authorValidator->setRegularExpression(rx);
    ui->lineEditAuthor->setValidator(authorValidator);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgImportExportIges::~DlgImportExportIges() = default;

void DlgImportExportIges::saveSettings()
{
    Part::IGES::ImportExportSettings settings;

    int unit = ui->comboBoxUnits->currentIndex();
    settings.setUnit(static_cast<Part::Interface::Unit>(unit));
    settings.setBRepMode(bg->checkedId() == 1);

    // Import
    settings.setSkipBlankEntities(ui->checkSkipBlank->isChecked());

    // header info
    settings.setCompany(ui->lineEditCompany->text().toLatin1());
    settings.setAuthor(ui->lineEditAuthor->text().toLatin1());
}

void DlgImportExportIges::loadSettings()
{
    Part::IGES::ImportExportSettings settings;

    ui->comboBoxUnits->setCurrentIndex(static_cast<int>(settings.getUnit()));

    bool brep = settings.getBRepMode();
    if (brep) {
        ui->radioButtonBRepOn->setChecked(true);
    }
    else {
        ui->radioButtonBRepOff->setChecked(true);
    }

    // Import
    ui->checkSkipBlank->setChecked(settings.getSkipBlankEntities());

    // header info
    ui->lineEditCompany->setText(QString::fromStdString(settings.getCompany()));
    ui->lineEditAuthor->setText(QString::fromStdString(settings.getAuthor()));
    ui->lineEditProduct->setText(QString::fromStdString(settings.getProductName()));
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgImportExportIges::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

// ----------------------------------------------------------------------------

DlgImportExportStep::DlgImportExportStep(QWidget* parent)
    : PreferencePage(parent)
    , exportStep(new DlgExportStep(this))
    , importStep(new DlgImportStep(this))
    , headerStep(new DlgExportHeaderStep(this))
{
    setWindowTitle(QLatin1String("STEP"));
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    layout->addWidget(exportStep);
    layout->addWidget(importStep);
    layout->addWidget(headerStep);

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 82, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgImportExportStep::~DlgImportExportStep() = default;

void DlgImportExportStep::saveSettings()
{
    exportStep->saveSettings();
    importStep->saveSettings();
    headerStep->saveSettings();
}

void DlgImportExportStep::loadSettings()
{
    exportStep->loadSettings();
    importStep->loadSettings();
    headerStep->loadSettings();
}

void DlgImportExportStep::changeEvent(QEvent*)
{
    // do nothing
}


#include "moc_DlgSettingsGeneral.cpp"
