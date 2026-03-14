#ifndef PROPERTYITEMDELEGATE_H
#define PROPERTYITEMDELEGATE_H

#include <QItemDelegate>
#include <QPointer>

namespace Gui
{
namespace PropertyEditor
{

class PropertyEditorWidget;

class PropertyItemDelegate: public QItemDelegate
{
    Q_OBJECT

public:
    explicit PropertyItemDelegate(QObject* parent);
    ~PropertyItemDelegate() override;

    void paint(QPainter* painter, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(
        QEvent* event,
        QAbstractItemModel* model,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) override;

protected:
    bool eventFilter(QObject*, QEvent*) override;

    void valueChanged();

private:
    mutable QPointer<QWidget> propertyEditor;
    mutable QWidget* expressionEditor;
    mutable PropertyEditorWidget* userEditor = nullptr;
    mutable bool pressed;
    bool changed;
};

}  // namespace PropertyEditor
}  // namespace Gui

#endif  // PROPERTYITEMDELEGATE_H
