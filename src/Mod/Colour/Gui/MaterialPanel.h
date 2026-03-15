// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_MATERIAL_PANEL_H
#define COLOUR_GUI_MATERIAL_PANEL_H

#include <QWidget>
#include <QPointer>
#include <QDockWidget>

class QSlider;
class QLabel;
class QDoubleSpinBox;
class QGroupBox;
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
class PBR;
}

namespace ColourGui {

class MaterialPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialPanel(QWidget* parent = nullptr);
    ~MaterialPanel() override;

    void setColourMapObject(Colour::ColourMapObject* obj);
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void refreshMaterialValues();
    void onSelectionChanged();

private Q_SLOTS:
    void onMetallicChanged(double value);
    void onRoughnessChanged(double value);
    void onAlphaChanged(double value);
    void onResetMaterial();

private:
    QPointer<Colour::ColourMapObject> _colourMapObject;
    QPointer<Gui::SelectionObserver> _selectionObserver;
    int _selectedFaceIndex;

    QDoubleSpinBox* _metallicSlider;
    QDoubleSpinBox* _roughnessSlider;
    QDoubleSpinBox* _alphaSlider;
    QLabel* _metallicLabel;
    QLabel* _roughnessLabel;
    QLabel* _alphaLabel;
};

class MaterialPanelDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MaterialPanelDockWidget(QWidget* parent = nullptr);
    ~MaterialPanelDockWidget() override;

    MaterialPanel* panel() { return _panel; }

private:
    MaterialPanel* _panel;
};

}

#endif