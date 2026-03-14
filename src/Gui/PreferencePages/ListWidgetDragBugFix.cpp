#include <QDragMoveEvent>


#include "ListWidgetDragBugFix.h"


ListWidgetDragBugFix::ListWidgetDragBugFix(QWidget* parent)
    : QListWidget(parent)
{}

ListWidgetDragBugFix::~ListWidgetDragBugFix() = default;

void ListWidgetDragBugFix::dragMoveEvent(QDragMoveEvent* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPoint pos = event->pos();
#else
    QPoint pos = event->position().toPoint();
#endif
    if ((row(itemAt(pos)) == currentRow() + 1)
        || (currentRow() == count() - 1 && row(itemAt(pos)) == -1)) {
        event->ignore();
        return;
    }
    QListWidget::dragMoveEvent(event);
}

#include "moc_ListWidgetDragBugFix.cpp"
