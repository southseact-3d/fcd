// SPDX-License-Identifier: LGPL-2.1-or-later

#include "PanelManager.h"

#include "LayerPanel.h"
#include "MaterialPanel.h"

#include <Gui/MainWindow.h>

namespace ColourGui {

PanelManager* PanelManager::_instance = nullptr;

PanelManager::PanelManager()
    : QObject(nullptr)
{
    QWidget* mainWindow = Gui::getMainWindow();
    if (mainWindow) {
        _layerPanelDock = new LayerPanelDockWidget(mainWindow);
        _materialPanelDock = new MaterialPanelDockWidget(mainWindow);

        _layerPanelDock->hide();
        _materialPanelDock->hide();
    }
}

PanelManager::~PanelManager()
{
    if (_layerPanelDock) {
        _layerPanelDock->deleteLater();
    }
    if (_materialPanelDock) {
        _materialPanelDock->deleteLater();
    }
}

PanelManager& PanelManager::instance()
{
    if (!_instance) {
        _instance = new PanelManager();
    }
    return *_instance;
}

void PanelManager::showLayerPanel()
{
    if (_layerPanelDock) {
        _layerPanelDock->show();
        _layerPanelDock->raise();
        Q_EMIT layerPanelVisibilityChanged(true);
    }
}

void PanelManager::hideLayerPanel()
{
    if (_layerPanelDock) {
        _layerPanelDock->hide();
        Q_EMIT layerPanelVisibilityChanged(false);
    }
}

void PanelManager::toggleLayerPanel()
{
    if (_layerPanelDock && _layerPanelDock->isVisible()) {
        hideLayerPanel();
    } else {
        showLayerPanel();
    }
}

void PanelManager::showMaterialPanel()
{
    if (_materialPanelDock) {
        _materialPanelDock->show();
        _materialPanelDock->raise();
        Q_EMIT materialPanelVisibilityChanged(true);
    }
}

void PanelManager::hideMaterialPanel()
{
    if (_materialPanelDock) {
        _materialPanelDock->hide();
        Q_EMIT materialPanelVisibilityChanged(false);
    }
}

void PanelManager::toggleMaterialPanel()
{
    if (_materialPanelDock && _materialPanelDock->isVisible()) {
        hideMaterialPanel();
    } else {
        showMaterialPanel();
    }
}

bool PanelManager::isLayerPanelVisible() const
{
    return _layerPanelDock && _layerPanelDock->isVisible();
}

bool PanelManager::isMaterialPanelVisible() const
{
    return _materialPanelDock && _materialPanelDock->isVisible();
}

}