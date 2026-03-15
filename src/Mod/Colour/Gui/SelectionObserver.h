// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_SELECTIONOBSERVER_H
#define COLOUR_GUI_SELECTIONOBSERVER_H

#include <Gui/SelectionObserver.h>
#include <QObject>
#include <QColor>
#include <cstring>

class QColor;

namespace ColourGui
{

class SelectionObserver: public QObject, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    SelectionObserver();
    ~SelectionObserver() override;

    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

    void setActivePaintMode(bool active);
    bool isActivePaintMode() const;

    void setPaintColor(const QColor& color);
    QColor getPaintColor() const;

Q_SIGNALS:
    void faceSelected(int faceIndex, App::DocumentObject* object);
    void paintModeChanged(bool active);

private:
    bool m_paintModeActive;
    QColor m_paintColor;
    App::DocumentObject* m_currentObject;
};

}  // namespace ColourGui

#endif  // COLOUR_GUI_SELECTIONOBSERVER_H
