// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_EXPORTDIALOG_H
#define COLOUR_GUI_EXPORTDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QLabel>

namespace ColourGui
{

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget* parent = nullptr);
    ~ExportDialog() override = default;

    enum ExportFormat { Format3MF, FormatGLB, FormatOBJ };
    enum class Preset { None, JLC3DP };

    ExportFormat getFormat() const;
    Preset getPreset() const;
    int getAtlasResolution() const;
    bool isJlcWjpPreset() const;
    bool isValidateWallThickness() const;
    bool isForceOpaqueAlpha() const;
    bool isSrgbTextures() const;

private slots:
    void onFormatChanged(int index);
    void onPresetChanged(int index);

private:
    void setupUi();
    void updatePresetOptions();

    QRadioButton* format3mf;
    QRadioButton* formatGlb;
    QRadioButton* formatObj;
    
    QComboBox* presetCombo;
    QSpinBox* resolutionSpin;
    QCheckBox* validateThickness;
    QCheckBox* forceOpaque;
    QCheckBox* srgbTextures;

    int currentResolution;
    Preset currentPreset;
};

}  // namespace ColourGui

#endif  // COLOUR_GUI_EXPORTDIALOG_H
