// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SelectionObserver.h"

#include <App/DocumentObject.h>
#include <Gui/Selection.h>

#include <Mod/Colour/App/ColourMapObject.h>

using namespace ColourGui;

SelectionObserver::SelectionObserver()
    : m_paintModeActive(false)
    , m_paintColor(255, 255, 255)
    , m_currentObject(nullptr)
{
    Gui::Selection().addSelectionObserver(this);
}

SelectionObserver::~SelectionObserver()
{
    Gui::Selection().removeSelectionObserver(this);
}

void SelectionObserver::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (!m_paintModeActive) {
        return;
    }

    if (msg.Type == Gui::SelectionChanges::AddSelection ||
        msg.Type == Gui::SelectionChanges::SetSelection) {
        
        App::DocumentObject* obj = msg.pObject;
        if (obj && obj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            m_currentObject = obj;
            
            int faceIndex = 0;
            if (msg.pSubName && strlen(msg.pSubName) > 0) {
                if (strncmp(msg.pSubName, "Face", 4) == 0) {
                    faceIndex = atoi(msg.pSubName + 4) - 1;
                }
            }
            
            Q_EMIT faceSelected(faceIndex, obj);
        }
    }
}

void SelectionObserver::setActivePaintMode(bool active)
{
    m_paintModeActive = active;
    Q_EMIT paintModeChanged(active);
}

bool SelectionObserver::isActivePaintMode() const
{
    return m_paintModeActive;
}

void SelectionObserver::setPaintColor(const QColor& color)
{
    m_paintColor = color;
}

QColor SelectionObserver::getPaintColor() const
{
    return m_paintColor;
}
