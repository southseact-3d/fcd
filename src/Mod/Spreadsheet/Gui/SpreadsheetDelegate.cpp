#include <QLineEdit>
#include <QPainter>


#include <App/DocumentObject.h>
#include <Base/Console.h>
#include <Mod/Spreadsheet/App/Sheet.h>

#include "LineEdit.h"
#include "SpreadsheetDelegate.h"


FC_LOG_LEVEL_INIT("Spreadsheet", true, true)

using namespace Spreadsheet;
using namespace SpreadsheetGui;

SpreadsheetDelegate::SpreadsheetDelegate(Spreadsheet::Sheet* _sheet, QWidget* parent)
    : QStyledItemDelegate(parent)
    , sheet(_sheet)
{}

QWidget* SpreadsheetDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index
) const
{
    App::CellAddress addr(index.row(), index.column());
    App::Range range(addr, addr);
    if (sheet && sheet->getCellBinding(range)) {
        FC_ERR("Bound cell " << addr.toString() << " cannot be edited");
        return nullptr;
    }

    SpreadsheetGui::LineEdit* editor = new SpreadsheetGui::LineEdit(parent);
    editor->setDocumentObject(sheet);
    connect(
        editor,
        &SpreadsheetGui::LineEdit::finishedWithKey,
        this,
        &SpreadsheetDelegate::onEditorFinishedWithKey
    );
    return editor;
}

void SpreadsheetDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
}

void SpreadsheetDelegate::setModelData(
    QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index
) const
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        model->setData(index, edit->text());
        return;
    }
}

void SpreadsheetDelegate::onEditorFinishedWithKey(int key, Qt::KeyboardModifiers modifiers)
{
    Q_EMIT finishedWithKey(key, modifiers);
}

static inline void drawBorder(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    unsigned flags,
    QColor color,
    Qt::PenStyle style
)
{
    if (!flags) {
        return;
    }
    QPen pen(color);
    pen.setWidth(2);
    pen.setStyle(style);
    painter->setPen(pen);

    QRect rect = option.rect.adjusted(1, 1, 0, 0);
    if (flags == Sheet::BorderAll) {
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        return;
    }
    if (flags & Sheet::BorderLeft) {
        painter->drawLine(rect.topLeft(), rect.bottomLeft());
    }
    if (flags & Sheet::BorderTop) {
        painter->drawLine(rect.topLeft(), rect.topRight());
    }
    if (flags & Sheet::BorderRight) {
        painter->drawLine(rect.topRight(), rect.bottomRight());
    }
    if (flags & Sheet::BorderBottom) {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }
}

void SpreadsheetDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const
{
    QStyledItemDelegate::paint(painter, option, index);
    if (!sheet) {
        return;
    }
    App::CellAddress addr(index.row(), index.column());
    drawBorder(painter, option, sheet->getCellBindingBorder(addr), Qt::blue, Qt::SolidLine);
    drawBorder(painter, option, sheet->getCopyOrCutBorder(addr, true), Qt::green, Qt::DashLine);
    drawBorder(painter, option, sheet->getCopyOrCutBorder(addr, false), Qt::red, Qt::DashLine);
}

#include "moc_SpreadsheetDelegate.cpp"
