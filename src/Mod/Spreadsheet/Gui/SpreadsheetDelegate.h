#ifndef SPREADSHEETDELEGATE_H
#define SPREADSHEETDELEGATE_H

#include <QStyledItemDelegate>

namespace Spreadsheet
{
class Sheet;
}

namespace SpreadsheetGui
{

class SpreadsheetDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SpreadsheetDelegate(Spreadsheet::Sheet* sheet, QWidget* parent = nullptr);
    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex& index
    ) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) const override;

Q_SIGNALS:
    void finishedWithKey(int key, Qt::KeyboardModifiers modifiers);

private:
    void onEditorFinishedWithKey(int key, Qt::KeyboardModifiers modifiers);

private:
    Spreadsheet::Sheet* sheet;
};

}  // namespace SpreadsheetGui

#endif  // SPREADSHEETDELEGATE_H
