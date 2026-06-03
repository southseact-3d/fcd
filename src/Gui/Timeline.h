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
#include <QTimer>
#include <QSet>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <set>

namespace App
{
class Document;
class DocumentObject;
}

namespace Gui
{

class TimelineWidgetItem;
class TimelineSlider;
class TimelineGroup;

struct TimelineOperation
{
    std::string id;
    std::string name;
    std::string type;
    QIcon icon;
    int index;
    bool isCurrent;
    bool isSuppressed;
    bool isRolledBack;
    std::string documentName;
    std::string objectName;
    int groupIndex; // -1 if not in a group
};

class TimelineWidgetItem : public QToolButton
{
    Q_OBJECT

public:
    explicit TimelineWidgetItem(const TimelineOperation& op, QWidget* parent = nullptr);
    void setIcon(const QIcon& icon);
    void setCurrent(bool current);
    void setSuppressed(bool suppressed);
    void setRolledBack(bool rolledBack);
    bool isCurrent() const { return m_isCurrent; }
    bool isSuppressed() const { return m_isSuppressed; }
    const TimelineOperation& operation() const { return m_operation; }

Q_SIGNALS:
    void editRequested(const TimelineOperation& op);
    void deleteRequested(const TimelineOperation& op);
    void suppressToggled(const TimelineOperation& op, bool suppressed);
    void rollbackHere(const TimelineOperation& op);
    void dragStarted(int index);
    void dragDropped(int fromIndex, int toIndex);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void updateStyle();
    TimelineOperation m_operation;
    bool m_isCurrent;
    bool m_isSuppressed;
    QPoint m_dragStartPosition;
};

class TimelineGroup : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineGroup(const QString& name, int startIndex, QWidget* parent = nullptr);

    void addWidget(TimelineWidgetItem* widget);
    void removeWidget(TimelineWidgetItem* widget);
    const std::vector<TimelineWidgetItem*>& widgets() const { return m_widgets; }
    QString groupName() const { return m_name; }
    int startIndex() const { return m_startIndex; }
    bool isExpanded() const { return m_expanded; }

    void setExpanded(bool expanded);
    void setName(const QString& name);

Q_SIGNALS:
    void groupExpanded(const QString& name, bool expanded);

private:
    QString m_name;
    int m_startIndex;
    bool m_expanded;
    QToolButton* m_expandButton;
    QLabel* m_nameLabel;
    QWidget* m_contentWidget;
    QHBoxLayout* m_contentLayout;
    std::vector<TimelineWidgetItem*> m_widgets;
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

    bool isPlaying() const;

public Q_SLOTS:
    void goToStart();
    void goToEnd();
    void goToPrevious();
    void goToNext();
    void startPlayback();
    void stopPlayback();
    void onOperationEdited(const TimelineOperation& op);
    void onOperationDeleted(const TimelineOperation& op);
    void onOperationSuppressToggled(const TimelineOperation& op, bool suppressed);
    void onOperationRollbackHere(const TimelineOperation& op);
    void onSliderMoved(int position);
    void onSliderReleased();
    void onDragDropped(int fromIndex, int toIndex);

Q_SIGNALS:
    void positionChanged(int position);
    void operationSelected(const TimelineOperation& op);
    void timelineChanged();

private:
    void setupUI();
    void setupPlaybackControls();
    void loadOperationIcons();
    void updateOperationsList();
    void updateSliderRange();
    void applyStateAtPosition(int position);
    void restoreFullState();
    QIcon getIconForOperation(const std::string& type) const;
    bool canMoveOperation(int fromIndex, int toIndex) const;
    void performReorder(int fromIndex, int toIndex);

    TimelineSlider* m_slider;
    QScrollArea* m_operationsScroll;
    QWidget* m_operationsContainer;
    QHBoxLayout* m_operationsLayout;
    QLabel* m_positionLabel;
    QToolButton* m_btnStart;
    QToolButton* m_btnEnd;
    QToolButton* m_btnPrev;
    QToolButton* m_btnNext;
    QToolButton* m_btnPlay;

    std::vector<TimelineOperation> m_operations;
    std::vector<TimelineWidgetItem*> m_operationWidgets;
    std::map<int, TimelineGroup*> m_groups; // startIndex -> group
    App::Document* m_document;
    int m_currentPosition;
    bool m_isDragging;
    bool m_isPlaying;
    QTimer* m_playbackTimer;

    std::map<std::string, QIcon> m_operationIcons;
    QSet<App::DocumentObject*> m_timelineSuppressedObjects;

    fastsignals::scoped_connection connectNewObject;
    fastsignals::scoped_connection connectDelObject;
    fastsignals::scoped_connection connectUndo;
    fastsignals::scoped_connection connectRedo;
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
