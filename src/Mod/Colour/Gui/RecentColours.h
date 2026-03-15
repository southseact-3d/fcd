// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_RECENTCOLOURS_H
#define COLOUR_GUI_RECENTCOLOURS_H

#include <QColor>
#include <QList>
#include <QObject>

namespace ColourGui
{

class RecentColours: public QObject
{
    Q_OBJECT

public:
    static RecentColours& instance();

    void addColour(const QColor& colour);
    QList<QColor> getRecentColours() const;
    void clear();

    static constexpr int MaxRecentColours = 10;

Q_SIGNALS:
    void coloursChanged();

private:
    RecentColours() = default;
    ~RecentColours() = default;
    RecentColours(const RecentColours&) = delete;
    RecentColours& operator=(const RecentColours&) = delete;

    QList<QColor> m_recentColours;
};

}  // namespace ColourGui

#endif  // COLOUR_GUI_RECENTCOLOURS_H
