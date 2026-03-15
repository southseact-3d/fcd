// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_COLOUR_PICKER_DIALOG_H
#define COLOUR_GUI_COLOUR_PICKER_DIALOG_H

#include <QDialog>
#include <QColor>

class QLineEdit;
class QSpinBox;
class QLabel;
class QPushButton;
class QComboBox;
class QGridLayout;

namespace ColourGui {

class ColourPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColourPickerDialog(const QColor& initialColor, QWidget* parent = nullptr);
    ~ColourPickerDialog() override;

    QColor selectedColor() const { return _currentColor; }

private:
    void updateColorFromRGB();
    void updateColorFromHSV();
    void updateColorFromHex();
    void updateColorDisplay();
    void addToRecentColours();

private Q_SLOTS:
    void onRedChanged(int value);
    void onGreenChanged(int value);
    void onBlueChanged(int value);
    void onAlphaChanged(int value);
    void onHueChanged(int value);
    void onSaturationChanged(int value);
    void onValueChanged(int value);
    void onHexEditingFinished();
    void onModeChanged(int index);
    void onRecentColourClicked();

private:
    QColor _currentColor;
    bool _updating;

    QSpinBox* _redSpin;
    QSpinBox* _greenSpin;
    QSpinBox* _blueSpin;
    QSpinBox* _alphaSpin;
    QSpinBox* _hueSpin;
    QSpinBox* _saturationSpin;
    QSpinBox* _valueSpin;
    QLineEdit* _hexEdit;
    QLabel* _colorPreview;
    QComboBox* _modeCombo;
    QWidget* _rgbPanel;
    QWidget* _hsvPanel;
    QGridLayout* _recentGrid;
    std::vector<QColor> _recentColours;
};

}

#endif