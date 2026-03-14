#include <QApplication>
#include <QPainter>


#include <QStyleOptionViewItem>

#include "DAGRectItem.h"


using namespace Gui;
using namespace DAG;

RectItem::RectItem(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    selected = false;
    preSelected = false;
    editing = false;
}

void RectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();

    QStyleOptionViewItem styleOption;

    styleOption.backgroundBrush = backgroundBrush;
    if (editing) {
        styleOption.backgroundBrush = editBrush;
    }
    else {
        styleOption.state |= QStyle::State_Enabled;
        if (selected) {
            styleOption.state |= QStyle::State_Selected;
        }
        if (preSelected) {
            if (!selected) {
                styleOption.state |= QStyle::State_Selected;
                QPalette palette = styleOption.palette;
                QColor tempColor = palette.color(QPalette::Active, QPalette::Highlight);
                tempColor.setAlphaF(0.15F);
                palette.setColor(QPalette::Inactive, QPalette::Highlight, tempColor);
                styleOption.palette = palette;
            }
            styleOption.state |= QStyle::State_MouseOver;
        }
    }
    styleOption.rect = this->rect().toRect();

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    painter->restore();
}
