#ifndef QLISTWIDGETDRAGBUGFIX_HPP
#define QLISTWIDGETDRAGBUGFIX_HPP

#include <QDragMoveEvent>
#include <QListWidget>

/* Qt has a recent bug (2023, https://bugreports.qt.io/browse/QTBUG-100128)
 * where the items disappears in certain conditions when drag and dropping.
 * Here we prevent the situation where this happens.
 * 1 - If the item is dropped on the item below such that the item doesn't move (ie superior half of
 * the below item) 2 - The item is the last one and user drop it on the empty space below. In both
 * those cases the item widget was lost. When Qt solve this bug, this class should not be required
 * anymore.
 */
class ListWidgetDragBugFix: public QListWidget
{
    Q_OBJECT

public:
    explicit ListWidgetDragBugFix(QWidget* parent);
    ~ListWidgetDragBugFix() override;

protected:
    void dragMoveEvent(QDragMoveEvent* e) override;
};

#endif
