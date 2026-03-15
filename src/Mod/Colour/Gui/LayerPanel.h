// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_LAYER_PANEL_H
#define COLOUR_GUI_LAYER_PANEL_H

#include <QWidget>
#include <QPointer>
#include <QDockWidget>
#include <vector>

class QListWidget;
class QShowEvent;
class QHideEvent;

namespace App {
class DocumentObject;
}

namespace Gui {
class SelectionObserver;
}

namespace Colour {
class ColourMapObject;
}

namespace ColourGui {

class LayerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayerPanel(QWidget* parent = nullptr);
    ~LayerPanel() override;

    void setColourMapObject(Colour::ColourMapObject* obj);
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void refreshLayerList();
    void onSelectionChanged();
    void onLayerChanged();

private Q_SLOTS:
    void onAddLayer();
    void onDeleteLayer();
    void onLayerVisibilityToggled(int index);
    void onLayerSelected(int index);
    void onMoveLayerUp();
    void onMoveLayerDown();
    void onLayerNameChanged(int index, const QString& name);

private:
    QPointer<Colour::ColourMapObject> _colourMapObject;
    QPointer<Gui::SelectionObserver> _selectionObserver;
    int _selectedFaceIndex;
    std::size_t _selectedLayerIndex;

    class SelectionListener;
    QListWidget* _layerList;
};

class LayerPanelDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LayerPanelDockWidget(QWidget* parent = nullptr);
    ~LayerPanelDockWidget() override;

    LayerPanel* panel() { return _panel; }

private:
    LayerPanel* _panel;
};

}

#endif