// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ExportDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

namespace ColourGui
{

ExportDialog::ExportDialog(QWidget* parent)
    : QDialog(parent)
    , currentResolution(2048)
    , currentPreset(Preset::None)
{
    setupUi();
    setWindowTitle(tr("Export Textured Model"));
    setMinimumWidth(400);
}

void ExportDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox* formatGroup = new QGroupBox(tr("Export Format"), this);
    QVBoxLayout* formatLayout = new QVBoxLayout(formatGroup);
    
    format3mf = new QRadioButton(tr("3MF (recommended for JLC3DP)"), formatGroup);
    formatGlb = new QRadioButton(tr("GLB (glTF Binary)"), formatGroup);
    formatObj = new QRadioButton(tr("OBJ + MTL"), formatGroup);
    
    format3mf->setChecked(true);
    
    formatLayout->addWidget(format3mf);
    formatLayout->addWidget(formatGlb);
    formatLayout->addWidget(formatObj);
    
    mainLayout->addWidget(formatGroup);

    QGroupBox* presetGroup = new QGroupBox(tr("Export Preset"), this);
    QVBoxLayout* presetLayout = new QVBoxLayout(presetGroup);
    
    presetCombo = new QComboBox(presetGroup);
    presetCombo->addItem(tr("None"), static_cast<int>(Preset::None));
    presetCombo->addItem(tr("JLC3DP WJP (Wax Jetting)"), static_cast<int>(Preset::JLC3DP));
    
    presetLayout->addWidget(presetGroup);
    mainLayout->addWidget(presetGroup);

    QGroupBox* optionsGroup = new QGroupBox(tr("Options"), this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    QHBoxLayout* resLayout = new QHBoxLayout();
    QLabel* resLabel = new QLabel(tr("Atlas Resolution:"), optionsGroup);
    resolutionSpin = new QSpinBox(optionsGroup);
    resolutionSpin->setRange(512, 4096);
    resolutionSpin->setValue(2048);
    resolutionSpin->setSingleStep(512);
    resLayout->addWidget(resLabel);
    resLayout->addWidget(resolutionSpin);
    optionsLayout->addLayout(resLayout);
    
    validateThickness = new QCheckBox(tr("Validate wall thickness for JLC3DP"), optionsGroup);
    validateThickness->setChecked(true);
    optionsLayout->addWidget(validateThickness);
    
    forceOpaque = new QCheckBox(tr("Force opaque alpha"), optionsGroup);
    optionsLayout->addWidget(forceOpaque);
    
    srgbTextures = new QCheckBox(tr("sRGB color space"), optionsGroup);
    srgbTextures->setChecked(true);
    optionsLayout->addWidget(srgbTextures);
    
    mainLayout->addWidget(optionsGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    connect(presetCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ExportDialog::onPresetChanged);
}

void ExportDialog::onPresetChanged(int index)
{
    currentPreset = static_cast<Preset>(presetCombo->itemData(index).toInt());
    updatePresetOptions();
}

void ExportDialog::updatePresetOptions()
{
    if (currentPreset == Preset::JLC3DP) {
        format3mf->setChecked(true);
        format3mf->setEnabled(false);
        formatGlb->setEnabled(false);
        formatObj->setEnabled(false);
        forceOpaque->setChecked(true);
        forceOpaque->setEnabled(false);
        validateThickness->setChecked(true);
        validateThickness->setEnabled(false);
    }
    else {
        format3mf->setEnabled(true);
        formatGlb->setEnabled(true);
        formatObj->setEnabled(true);
        forceOpaque->setEnabled(true);
        validateThickness->setEnabled(true);
    }
}

ExportDialog::ExportFormat ExportDialog::getFormat() const
{
    if (format3mf->isChecked()) return Format3MF;
    if (formatGlb->isChecked()) return FormatGLB;
    return FormatOBJ;
}

ExportDialog::Preset ExportDialog::getPreset() const
{
    return currentPreset;
}

int ExportDialog::getAtlasResolution() const
{
    return resolutionSpin->value();
}

bool ExportDialog::isJlcWjpPreset() const
{
    return currentPreset == Preset::JLC3DP;
}

bool ExportDialog::isValidateWallThickness() const
{
    return validateThickness->isChecked();
}

bool ExportDialog::isForceOpaqueAlpha() const
{
    return forceOpaque->isChecked();
}

bool ExportDialog::isSrgbTextures() const
{
    return srgbTextures->isChecked();
}

}  // namespace ColourGui
