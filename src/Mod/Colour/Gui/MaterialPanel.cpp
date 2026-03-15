// SPDX-License-Identifier: LGPL-2.1-or-later

#include "MaterialPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Selection.h>
#include <Gui/SelectionObserver.h>

#include <Mod/Colour/App/ColourMap.h>
#include <Mod/Colour/App/ColourMapObject.h>

namespace ColourGui {

class MaterialPanel::SelectionListener : public Gui::SelectionObserver
{
public:
    explicit SelectionListener(MaterialPanel* panel)
        : _panel(panel)
    {}

    void onSelectionChanged(const Gui::SelectionChanges& msg) override
    {
        if (_panel) {
            _panel->onSelectionChanged();
        }
    }

private:
    MaterialPanel* _panel;
};

MaterialPanel::MaterialPanel(QWidget* parent)
    : QWidget(parent)
    , _selectedFaceIndex(0)
{
    setWindowTitle(QStringLiteral("Material Panel"));

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    auto* titleLabel = new QLabel(tr("PBR Material Properties"), this);
    titleLabel->setStyleSheet(QStringLiteral("font-weight: bold;"));
    mainLayout->addWidget(titleLabel);

    auto* infoLabel = new QLabel(tr("Select a face in the viewport to edit its material properties."), this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(QStringLiteral("color: gray; font-size: 10px;"));
    mainLayout->addWidget(infoLabel);

    mainLayout->addSpacing(10);

    auto* groupBox = new QGroupBox(tr("Per-Face PBR Values"), this);
    auto* groupLayout = new QGridLayout(groupBox);

    int row = 0;

    auto* metallicLabel = new QLabel(tr("Metallic:"), this);
    groupLayout->addWidget(metallicLabel, row, 0);

    _metallicSlider = new QDoubleSpinBox(this);
    _metallicSlider->setRange(0.0, 1.0);
    _metallicSlider->setSingleStep(0.05);
    _metallicSlider->setDecimals(2);
    _metallicSlider->setToolTip(tr("Metallic factor (0.0 = dielectric, 1.0 = metal)"));
    connect(_metallicSlider, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &MaterialPanel::onMetallicChanged);
    groupLayout->addWidget(_metallicSlider, row, 1);
    row++;

    auto* roughnessLabel = new QLabel(tr("Roughness:"), this);
    groupLayout->addWidget(roughnessLabel, row, 0);

    _roughnessSlider = new QDoubleSpinBox(this);
    _roughnessSlider->setRange(0.0, 1.0);
    _roughnessSlider->setSingleStep(0.05);
    _roughnessSlider->setDecimals(2);
    _roughnessSlider->setToolTip(tr("Roughness factor (0.0 = smooth, 1.0 = rough)"));
    connect(_roughnessSlider, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &MaterialPanel::onRoughnessChanged);
    groupLayout->addWidget(_roughnessSlider, row, 1);
    row++;

    auto* alphaLabel = new QLabel(tr("Alpha:"), this);
    groupLayout->addWidget(alphaLabel, row, 0);

    _alphaSlider = new QDoubleSpinBox(this);
    _alphaSlider->setRange(0.0, 1.0);
    _alphaSlider->setSingleStep(0.05);
    _alphaSlider->setDecimals(2);
    _alphaSlider->setToolTip(tr("Opacity (1.0 = fully opaque, 0.0 = fully transparent)"));
    connect(_alphaSlider, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &MaterialPanel::onAlphaChanged);
    groupLayout->addWidget(_alphaSlider, row, 1);
    row++;

    mainLayout->addWidget(groupBox);

    mainLayout->addSpacing(10);

    auto* resetBtn = new QPushButton(tr("Reset to Default"), this);
    resetBtn->setToolTip(tr("Reset material to default values (non-metallic, medium roughness)"));
    connect(resetBtn, &QPushButton::clicked, this, &MaterialPanel::onResetMaterial);
    mainLayout->addWidget(resetBtn);

    mainLayout->addStretch();

    auto* noteLabel = new QLabel(
        tr("Note: For JLC3DP WJP export, alpha is forced to 1.0 (opaque)."), this);
    noteLabel->setWordWrap(true);
    noteLabel->setStyleSheet(QStringLiteral("color: gray; font-size: 9px;"));
    mainLayout->addWidget(noteLabel);

    _selectionObserver = new SelectionListener(this);
    Gui::Selection().addObserver(_selectionObserver);
}

MaterialPanel::~MaterialPanel()
{
    if (_selectionObserver) {
        Gui::Selection().removeObserver(_selectionObserver);
        delete _selectionObserver;
    }
}

void MaterialPanel::setColourMapObject(Colour::ColourMapObject* obj)
{
    _colourMapObject = obj;
    refreshMaterialValues();
}

void MaterialPanel::showEvent(QShowEvent*)
{
    onSelectionChanged();
}

void MaterialPanel::hideEvent(QHideEvent*)
{
}

void MaterialPanel::refreshMaterialValues()
{
    if (!_colourMapObject) {
        _metallicSlider->setValue(0.0);
        _roughnessSlider->setValue(0.5);
        _alphaSlider->setValue(1.0);
        return;
    }

    const Colour::ColourMap& map = _colourMapObject->getColourMap();
    Colour::PBR pbr = map.getFacePbr(_selectedFaceIndex);

    _metallicSlider->setValue(pbr.metallic);
    _roughnessSlider->setValue(pbr.roughness);
    _alphaSlider->setValue(pbr.alpha);
}

void MaterialPanel::onSelectionChanged()
{
    auto& selection = Gui::Selection();
    std::vector<App::DocumentObject*> sel = selection.getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (!sel.empty()) {
        _colourMapObject = static_cast<Colour::ColourMapObject*>(sel.front());

        const QString& selStr = selection.getSelection();
        if (selStr.contains(QStringLiteral("Face"))) {
            int faceIdx = 0;
            QRegExp rx(QStringLiteral("Face(\\d+)"));
            if (rx.indexIn(selStr) != -1) {
                faceIdx = rx.cap(1).toInt();
            }
            _selectedFaceIndex = faceIdx;
        }

        refreshMaterialValues();
    }
}

void MaterialPanel::onMetallicChanged(double value)
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    Colour::PBR pbr = map.getFacePbr(_selectedFaceIndex);
    pbr.metallic = static_cast<float>(value);
    map.setFacePbr(_selectedFaceIndex, pbr);
    _colourMapObject->setColourMap(map);
    _colourMapObject->recompute();
}

void MaterialPanel::onRoughnessChanged(double value)
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    Colour::PBR pbr = map.getFacePbr(_selectedFaceIndex);
    pbr.roughness = static_cast<float>(value);
    map.setFacePbr(_selectedFaceIndex, pbr);
    _colourMapObject->setColourMap(map);
    _colourMapObject->recompute();
}

void MaterialPanel::onAlphaChanged(double value)
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    Colour::PBR pbr = map.getFacePbr(_selectedFaceIndex);
    pbr.alpha = static_cast<float>(value);
    map.setFacePbr(_selectedFaceIndex, pbr);
    _colourMapObject->setColourMap(map);
    _colourMapObject->recompute();
}

void MaterialPanel::onResetMaterial()
{
    if (!_colourMapObject) {
        QMessageBox::warning(this, tr("Material Panel"), tr("No ColourMap selected."));
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    Colour::PBR pbr;
    pbr.metallic = 0.0F;
    pbr.roughness = 0.5F;
    pbr.alpha = 1.0F;
    map.setFacePbr(_selectedFaceIndex, pbr);
    _colourMapObject->setColourMap(map);
    _colourMapObject->recompute();

    refreshMaterialValues();
}

MaterialPanelDockWidget::MaterialPanelDockWidget(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Colour Material"));
    setObjectName(QStringLiteral("Colour_MaterialPanel"));

    _panel = new MaterialPanel(this);
    setWidget(_panel);

    setMinimumWidth(220);
    setMaximumWidth(280);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

MaterialPanelDockWidget::~MaterialPanelDockWidget() = default;

}