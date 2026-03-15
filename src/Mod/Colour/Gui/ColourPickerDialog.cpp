// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ColourPickerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QColorDialog>

#include "RecentColours.h"

namespace ColourGui {

static const int MaxRecentColours = 10;

ColourPickerDialog::ColourPickerDialog(const QColor& initialColor, QWidget* parent)
    : QDialog(parent)
    , _currentColor(initialColor)
    , _updating(false)
{
    setWindowTitle(tr("Colour Picker"));
    setMinimumSize(400, 350);

    _recentColours = RecentColours::instance().getRecentColours();

    auto* mainLayout = new QVBoxLayout(this);

    auto* topLayout = new QHBoxLayout;

    _colorPreview = new QLabel(this);
    _colorPreview->setFixedSize(80, 80);
    _colorPreview->setFrameStyle(QFrame::Box | QFrame::Sunken);
    _colorPreview->setStyleSheet(QStringLiteral("background-color: %1;")
        .arg(_currentColor.name()));
    topLayout->addWidget(_colorPreview);

    auto* rightLayout = new QVBoxLayout;

    _modeCombo = new QComboBox(this);
    _modeCombo->addItem(QStringLiteral("RGB"));
    _modeCombo->addItem(QStringLiteral("HSV"));
    connect(_modeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &ColourPickerDialog::onModeChanged);
    rightLayout->addWidget(_modeCombo);

    auto* hexLabel = new QLabel(tr("Hex:"), this);
    rightLayout->addWidget(hexLabel);

    _hexEdit = new QLineEdit(this);
    _hexEdit->setText(_currentColor.name());
    _hexEdit->setInputMask(QStringLiteral("HHHHHH"));
    _hexEdit->setMaxLength(6);
    connect(_hexEdit, &QLineEdit::editingFinished,
            this, &ColourPickerDialog::onHexEditingFinished);
    rightLayout->addWidget(_hexEdit);

    rightLayout->addStretch();
    topLayout->addLayout(rightLayout);

    mainLayout->addLayout(topLayout);

    _rgbPanel = new QWidget(this);
    auto* rgbLayout = new QGridLayout(_rgbPanel);

    auto* rLabel = new QLabel(tr("Red:"), this);
    _redSpin = new QSpinBox(this);
    _redSpin->setRange(0, 255);
    _redSpin->setValue(_currentColor.red());
    connect(_redSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onRedChanged);
    rgbLayout->addWidget(rLabel, 0, 0);
    rgbLayout->addWidget(_redSpin, 0, 1);

    auto* gLabel = new QLabel(tr("Green:"), this);
    _greenSpin = new QSpinBox(this);
    _greenSpin->setRange(0, 255);
    _greenSpin->setValue(_currentColor.green());
    connect(_greenSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onGreenChanged);
    rgbLayout->addWidget(gLabel, 1, 0);
    rgbLayout->addWidget(_greenSpin, 1, 1);

    auto* bLabel = new QLabel(tr("Blue:"), this);
    _blueSpin = new QSpinBox(this);
    _blueSpin->setRange(0, 255);
    _blueSpin->setValue(_currentColor.blue());
    connect(_blueSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onBlueChanged);
    rgbLayout->addWidget(bLabel, 2, 0);
    rgbLayout->addWidget(_blueSpin, 2, 1);

    auto* aLabel = new QLabel(tr("Alpha:"), this);
    _alphaSpin = new QSpinBox(this);
    _alphaSpin->setRange(0, 255);
    _alphaSpin->setValue(_currentColor.alpha());
    connect(_alphaSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onAlphaChanged);
    rgbLayout->addWidget(aLabel, 3, 0);
    rgbLayout->addWidget(_alphaSpin, 3, 1);

    mainLayout->addWidget(_rgbPanel);

    _hsvPanel = new QWidget(this);
    auto* hsvLayout = new QGridLayout(_hsvPanel);

    int h, s, v;
    _currentColor.getHsv(&h, &s, &v);

    auto* hLabel = new QLabel(tr("Hue:"), this);
    _hueSpin = new QSpinBox(this);
    _hueSpin->setRange(0, 359);
    _hueSpin->setValue(h);
    connect(_hueSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onHueChanged);
    hsvLayout->addWidget(hLabel, 0, 0);
    hsvLayout->addWidget(_hueSpin, 0, 1);

    auto* sLabel = new QLabel(tr("Saturation:"), this);
    _saturationSpin = new QSpinBox(this);
    _saturationSpin->setRange(0, 255);
    _saturationSpin->setValue(s);
    connect(_saturationSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onSaturationChanged);
    hsvLayout->addWidget(sLabel, 1, 0);
    hsvLayout->addWidget(_saturationSpin, 1, 1);

    auto* vLabel = new QLabel(tr("Value:"), this);
    _valueSpin = new QSpinBox(this);
    _valueSpin->setRange(0, 255);
    _valueSpin->setValue(v);
    connect(_valueSpin, qOverload<int>(&QSpinBox::valueChanged),
            this, &ColourPickerDialog::onValueChanged);
    hsvLayout->addWidget(vLabel, 2, 0);
    hsvLayout->addWidget(_valueSpin, 2, 1);

    mainLayout->addWidget(_hsvPanel);
    _hsvPanel->hide();

    auto* recentLabel = new QLabel(tr("Recent Colours:"), this);
    mainLayout->addWidget(recentLabel);

    _recentGrid = new QGridLayout;
    for (int i = 0; i < static_cast<int>(_recentColours.size()) && i < MaxRecentColours; ++i) {
        auto* btn = new QPushButton(this);
        btn->setFixedSize(32, 32);
        btn->setStyleSheet(QStringLiteral("background-color: %1; border: 1px solid gray;")
            .arg(_recentColours[i].name()));
        btn->setToolTip(_recentColours[i].name());
        connect(btn, &QPushButton::clicked, this, &ColourPickerDialog::onRecentColourClicked);
        int row = i / 5;
        int col = i % 5;
        _recentGrid->addWidget(btn, row, col);
    }
    mainLayout->addLayout(_recentGrid);

    mainLayout->addStretch();

    auto* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

ColourPickerDialog::~ColourPickerDialog() = default;

void ColourPickerDialog::updateColorFromRGB()
{
    _currentColor.setRed(_redSpin->value());
    _currentColor.setGreen(_greenSpin->value());
    _currentColor.setBlue(_blueSpin->value());
    _currentColor.setAlpha(_alphaSpin->value());
    updateColorDisplay();
}

void ColourPickerDialog::updateColorFromHSV()
{
    _currentColor.setHsv(_hueSpin->value(),
                         _saturationSpin->value(),
                         _valueSpin->value());
    updateColorDisplay();
}

void ColourPickerDialog::updateColorFromHex()
{
    QString hex = _hexEdit->text();
    if (hex.length() == 6) {
        _currentColor = QColor(QStringLiteral("#") + hex);
        updateColorDisplay();
    }
}

void ColourPickerDialog::updateColorDisplay()
{
    if (_updating) return;
    _updating = true;

    _colorPreview->setStyleSheet(QStringLiteral("background-color: %1;")
        .arg(_currentColor.name()));
    _hexEdit->setText(_currentColor.name().remove(0, 1));

    int r, g, b, a;
    _currentColor.getRgb(&r, &g, &b, &a);
    _redSpin->setValue(r);
    _greenSpin->setValue(g);
    _blueSpin->setValue(b);
    _alphaSpin->setValue(a);

    int h, s, v;
    _currentColor.getHsv(&h, &s, &v);
    _hueSpin->setValue(h);
    _saturationSpin->setValue(s);
    _valueSpin->setValue(v);

    _updating = false;
}

void ColourPickerDialog::addToRecentColours()
{
    RecentColours::instance().addColour(_currentColor);
}

void ColourPickerDialog::onRedChanged(int)
{
    if (!_updating) updateColorFromRGB();
}

void ColourPickerDialog::onGreenChanged(int)
{
    if (!_updating) updateColorFromRGB();
}

void ColourPickerDialog::onBlueChanged(int)
{
    if (!_updating) updateColorFromRGB();
}

void ColourPickerDialog::onAlphaChanged(int)
{
    if (!_updating) updateColorFromRGB();
}

void ColourPickerDialog::onHueChanged(int)
{
    if (!_updating) updateColorFromHSV();
}

void ColourPickerDialog::onSaturationChanged(int)
{
    if (!_updating) updateColorFromHSV();
}

void ColourPickerDialog::onValueChanged(int)
{
    if (!_updating) updateColorFromHSV();
}

void ColourPickerDialog::onHexEditingFinished()
{
    if (!_updating) updateColorFromHex();
}

void ColourPickerDialog::onModeChanged(int index)
{
    if (index == 0) {
        _hsvPanel->hide();
        _rgbPanel->show();
    } else {
        _rgbPanel->hide();
        _hsvPanel->show();
    }
}

void ColourPickerDialog::onRecentColourClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    for (int i = 0; i < static_cast<int>(_recentColours.size()); ++i) {
        if (_recentColours[i].name() == btn->toolTip()) {
            _currentColor = _recentColours[i];
            updateColorDisplay();
            break;
        }
    }
}

void ColourPickerDialog::accept()
{
    addToRecentColours();
    QDialog::accept();
}

}