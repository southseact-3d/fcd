// SPDX-License-Identifier: LGPL-2.1-or-later

#include "LayerPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Selection.h>
#include <Gui/SelectionObserver.h>

#include <Mod/Colour/App/ColourMap.h>
#include <Mod/Colour/App/ColourMapObject.h>
#include "RecentColours.h"

namespace ColourGui {

class LayerPanel::SelectionListener : public Gui::SelectionObserver
{
public:
    explicit SelectionListener(LayerPanel* panel)
        : _panel(panel)
    {}

    void onSelectionChanged(const Gui::SelectionChanges& msg) override
    {
        if (_panel) {
            _panel->onSelectionChanged();
        }
    }

private:
    LayerPanel* _panel;
};

LayerPanel::LayerPanel(QWidget* parent)
    : QWidget(parent)
    , _selectedFaceIndex(0)
    , _selectedLayerIndex(0)
{
    setWindowTitle(QStringLiteral("Layer Panel"));

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    auto* headerLayout = new QHBoxLayout;
    auto* titleLabel = new QLabel(tr("Layers"), this);
    titleLabel->setStyleSheet(QStringLiteral("font-weight: bold;"));
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    auto* addBtn = new QPushButton(tr("+"), this);
    addBtn->setToolTip(tr("Add Layer"));
    addBtn->setMaximumWidth(24);
    connect(addBtn, &QPushButton::clicked, this, &LayerPanel::onAddLayer);
    headerLayout->addWidget(addBtn);

    auto* delBtn = new QPushButton(tr("-"), this);
    delBtn->setToolTip(tr("Delete Layer"));
    delBtn->setMaximumWidth(24);
    connect(delBtn, &QPushButton::clicked, this, &LayerPanel::onDeleteLayer);
    headerLayout->addWidget(delBtn);

    mainLayout->addLayout(headerLayout);

    _layerList = new QListWidget(this);
    _layerList->setSelectionMode(QListWidget::SingleSelection);
    connect(_layerList, &QListWidget::itemSelectionChanged, this, [this]() {
        if (_layerList->currentRow() >= 0) {
            _selectedLayerIndex = static_cast<std::size_t>(_layerList->currentRow());
        }
    });
    mainLayout->addWidget(_layerList);

    auto* btnLayout = new QHBoxLayout;
    auto* moveUpBtn = new QPushButton(tr("↑"), this);
    moveUpBtn->setToolTip(tr("Move Layer Up"));
    moveUpBtn->setMaximumWidth(30);
    connect(moveUpBtn, &QPushButton::clicked, this, &LayerPanel::onMoveLayerUp);
    btnLayout->addWidget(moveUpBtn);

    auto* moveDownBtn = new QPushButton(tr("↓"), this);
    moveDownBtn->setToolTip(tr("Move Layer Down"));
    moveDownBtn->setMaximumWidth(30);
    connect(moveDownBtn, &QPushButton::clicked, this, &LayerPanel::onMoveLayerDown);
    btnLayout->addWidget(moveDownBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    auto* infoLabel = new QLabel(tr("Select a face in the viewport to manage its layers."), this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(QStringLiteral("color: gray; font-size: 10px;"));
    mainLayout->addWidget(infoLabel);

    _selectionObserver = new SelectionListener(this);
    Gui::Selection().addObserver(_selectionObserver);
}

LayerPanel::~LayerPanel()
{
    if (_selectionObserver) {
        Gui::Selection().removeObserver(_selectionObserver);
        delete _selectionObserver;
    }
}

void LayerPanel::setColourMapObject(Colour::ColourMapObject* obj)
{
    _colourMapObject = obj;
    refreshLayerList();
}

void LayerPanel::showEvent(QShowEvent*)
{
    onSelectionChanged();
}

void LayerPanel::hideEvent(QHideEvent*)
{
}

void LayerPanel::refreshLayerList()
{
    _layerList->clear();

    if (!_colourMapObject) {
        return;
    }

    const Colour::ColourMap& map = _colourMapObject->getColourMap();
    const auto& layers = map.getLayers(_selectedFaceIndex);

    for (std::size_t i = 0; i < layers.size(); ++i) {
        const Colour::Layer& layer = layers[i];
        QString itemText = QString::fromStdString(layer.name);
        
        if (!layer.visible) {
            itemText = QStringLiteral("[H] ") + itemText;
        }
        if (layer.locked) {
            itemText = QStringLiteral("[L] ") + itemText;
        }
        if (layer.isImageLayer) {
            itemText = QStringLiteral("[I] ") + itemText;
        }

        auto* item = new QListWidgetItem(itemText, _layerList);
        
        QColor layerColor(
            static_cast<int>(layer.colour[0]),
            static_cast<int>(layer.colour[1]),
            static_cast<int>(layer.colour[2])
        );
        item->setBackground(layerColor);
    }
}

void LayerPanel::onSelectionChanged()
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
            _selectedFaceIndex = static_cast<std::size_t>(faceIdx);
        }

        refreshLayerList();
    }
}

void LayerPanel::onLayerChanged()
{
    if (_colourMapObject) {
        _colourMapObject->recompute();
        refreshLayerList();
    }
}

void LayerPanel::onAddLayer()
{
    if (!_colourMapObject) {
        QMessageBox::warning(this, tr("Layer Panel"), tr("No ColourMap selected."));
        return;
    }

    bool ok = false;
    QString name = QInputDialog::getText(
        this,
        tr("Add Layer"),
        tr("Layer name:"),
        QLineEdit::Normal,
        tr("New Layer"),
        &ok
    );

    if (!ok || name.isEmpty()) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();

    Colour::Layer layer;
    layer.name = name.toStdString();
    layer.zOrder = static_cast<int>(map.getLayers(_selectedFaceIndex).size());
    layer.visible = true;
    layer.locked = false;
    layer.isImageLayer = false;
    layer.colour = {128, 128, 128, 255};
    layer.pbr = {0.0F, 0.5F, 1.0F};

    map.addLayer(_selectedFaceIndex, layer);
    _colourMapObject->setColourMap(map);
    onLayerChanged();
}

void LayerPanel::onDeleteLayer()
{
    if (!_colourMapObject) {
        return;
    }

    const Colour::ColourMap& map = _colourMapObject->getColourMap();
    const auto& layers = map.getLayers(_selectedFaceIndex);

    if (layers.empty()) {
        QMessageBox::information(this, tr("Layer Panel"), tr("No layers to delete."));
        return;
    }

    int row = _layerList->currentRow();
    if (row < 0) {
        row = 0;
    }

    Colour::ColourMap mutableMap = map;
    mutableMap.removeLayer(_selectedFaceIndex, static_cast<std::size_t>(row));
    _colourMapObject->setColourMap(mutableMap);
    onLayerChanged();
}

void LayerPanel::onLayerVisibilityToggled(int index)
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    auto layers = map.getLayers(_selectedFaceIndex);

    if (index >= 0 && static_cast<std::size_t>(index) < layers.size()) {
        layers[static_cast<std::size_t>(index)].visible = !layers[static_cast<std::size_t>(index)].visible;
        map.setLayers(_selectedFaceIndex, layers);
        _colourMapObject->setColourMap(map);
        onLayerChanged();
    }
}

void LayerPanel::onLayerSelected(int index)
{
    _selectedLayerIndex = static_cast<std::size_t>(index);
}

void LayerPanel::onMoveLayerUp()
{
    if (!_colourMapObject || _selectedLayerIndex == 0) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    auto layers = map.getLayers(_selectedFaceIndex);

    if (_selectedLayerIndex > 0 && _selectedLayerIndex < layers.size()) {
        std::swap(layers[_selectedLayerIndex], layers[_selectedLayerIndex - 1]);
        map.setLayers(_selectedFaceIndex, layers);
        _colourMapObject->setColourMap(map);
        _selectedLayerIndex--;
        onLayerChanged();
    }
}

void LayerPanel::onMoveLayerDown()
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    auto layers = map.getLayers(_selectedFaceIndex);

    if (_selectedLayerIndex + 1 < layers.size()) {
        std::swap(layers[_selectedLayerIndex], layers[_selectedLayerIndex + 1]);
        map.setLayers(_selectedFaceIndex, layers);
        _colourMapObject->setColourMap(map);
        _selectedLayerIndex++;
        onLayerChanged();
    }
}

void LayerPanel::onLayerNameChanged(int index, const QString& name)
{
    if (!_colourMapObject) {
        return;
    }

    Colour::ColourMap map = _colourMapObject->getColourMap();
    auto layers = map.getLayers(_selectedFaceIndex);

    if (index >= 0 && static_cast<std::size_t>(index) < layers.size()) {
        layers[static_cast<std::size_t>(index)].name = name.toStdString();
        map.setLayers(_selectedFaceIndex, layers);
        _colourMapObject->setColourMap(map);
    }
}

LayerPanelDockWidget::LayerPanelDockWidget(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Colour Layers"));
    setObjectName(QStringLiteral("Colour_LayerPanel"));

    _panel = new LayerPanel(this);
    setWidget(_panel);

    setMinimumWidth(200);
    setMaximumWidth(300);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

LayerPanelDockWidget::~LayerPanelDockWidget() = default;

}