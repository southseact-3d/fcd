// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_PANEL_MANAGER_H
#define COLOUR_GUI_PANEL_MANAGER_H

#include <QObject>
#include <QPointer>

class QWidget;

namespace ColourGui {

class LayerPanelDockWidget;
class MaterialPanelDockWidget;

class PanelManager : public QObject
{
    Q_OBJECT

public:
    static PanelManager& instance();

    LayerPanelDockWidget* layerPanel() { return _layerPanelDock; }
    MaterialPanelDockWidget* materialPanel() { return _materialPanelDock; }

    void showLayerPanel();
    void hideLayerPanel();
    void toggleLayerPanel();

    void showMaterialPanel();
    void hideMaterialPanel();
    void toggleMaterialPanel();

    bool isLayerPanelVisible() const;
    bool isMaterialPanelVisible() const;

 Q_SIGNALS:
    void layerPanelVisibilityChanged(bool visible);
    void materialPanelVisibilityChanged(bool visible);

private:
    PanelManager();
    ~PanelManager() override;

    QPointer<LayerPanelDockWidget> _layerPanelDock;
    QPointer<MaterialPanelDockWidget> _materialPanelDock;

    static PanelManager* _instance;
};

}

#endif