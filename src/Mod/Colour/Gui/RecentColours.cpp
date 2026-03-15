// SPDX-License-Identifier: LGPL-2.1-or-later

#include "RecentColours.h"

using namespace ColourGui;

RecentColours& RecentColours::instance()
{
    static RecentColours instance;
    return instance;
}

void RecentColours::addColour(const QColor& colour)
{
    if (!colour.isValid()) {
        return;
    }

    m_recentColours.removeAll(colour);
    m_recentColours.prepend(colour);

    while (m_recentColours.size() > MaxRecentColours) {
        m_recentColours.removeLast();
    }

    Q_EMIT coloursChanged();
}

QList<QColor> RecentColours::getRecentColours() const
{
    return m_recentColours;
}

void RecentColours::clear()
{
    m_recentColours.clear();
    Q_EMIT coloursChanged();
}
