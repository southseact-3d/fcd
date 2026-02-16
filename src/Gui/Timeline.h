/***************************************************************************
 *   Copyright (c) 2025 Tungsten CAD Project                              *
 *                                                                         *
 *   This file is part of the Tungsten CAD development system.             *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef GUI_TIMELINE_H
#define GUI_TIMELINE_H

#include <QWidget>
#include <QSlider>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QFrame>
#include <QIcon>
#include <vector>
#include <string>
#include <memory>

namespace App
{
class Document;
class DocumentObject;
}

namespace Gui
{

class TimelineItem;
class TimelineSlider;

struct TimelineOperation
{
    std::string id;
    std::string name;
    std::string type;
    QIcon icon;
    int index;
    bool isCurrent;
    std::string documentName;
    std::string objectName;
};

class TimelineWidgetItem : public QToolButton
{
    Q_OBJECT

public:
    explicit TimelineWidgetItem(const TimelineOperation& op, QWidget* parent = nullptr);
    void setIcon(const QIcon& icon);
    void setCurrent(bool current);
    bool isCurrent() const { return m_isCurrent; }
    const TimelineOperation& operation() const { return m_operation; }

Q_SIGNALS:
    void editRequested(const TimelineOperation& op);
    void deleteRequested(const TimelineOperation& op);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    TimelineOperation m_operation;
    bool m_isCurrent;
};

class Timeline : public QWidget
{
    Q_OBJECT

public:
    explicit Timeline(QWidget* parent = nullptr);
    ~Timeline() override;

    void setDocument(App::Document* doc);
    void refresh();
    void clear();

    int currentPosition() const;
    void setCurrentPosition(int position);

    int operationCount() const;

public Q_SLOTS:
    void goToStart();
    void goToEnd();
    void goToPrevious();
    void goToNext();
    void onOperationEdited(const TimelineOperation& op);
    void onOperationDeleted(const TimelineOperation& op);
    void onSliderMoved(int position);
    void onSliderReleased();

Q_SIGNALS:
    void positionChanged(int position);
    void operationSelected(const TimelineOperation& op);
    void timelineChanged();

private:
    void setupUI();
    void loadOperationIcons();
    void updateOperationsList();
    void updateSliderRange();
    void applyStateAtPosition(int position);
    void restoreFullState();
    QIcon getIconForOperation(const std::string& type) const;

    TimelineSlider* m_slider;
    QScrollArea* m_operationsScroll;
    QWidget* m_operationsContainer;
    QHBoxLayout* m_operationsLayout;
    QLabel* m_positionLabel;
    QToolButton* m_btnStart;
    QToolButton* m_btnEnd;
    QToolButton* m_btnPrev;
    QToolButton* m_btnNext;

    std::vector<TimelineOperation> m_operations;
    std::vector<TimelineWidgetItem*> m_operationWidgets;
    App::Document* m_document;
    int m_currentPosition;
    bool m_isDragging;

    std::map<std::string, QIcon> m_operationIcons;
};

class TimelineSlider : public QSlider
{
    Q_OBJECT

public:
    explicit TimelineSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

Q_SIGNALS:
    void sliderMovedToPosition(int position);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};

} // namespace Gui

#endif // GUI_TIMELINE_H
