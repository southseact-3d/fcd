#ifndef MATGUI_ArrayDelegate_H
#define MATGUI_ArrayDelegate_H

#include <QDialog>
#include <QDir>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QSvgWidget>
#include <QTreeView>

#include <Mod/Material/App/MaterialManager.h>
#include <Mod/Material/App/Materials.h>
#include <Mod/Material/App/ModelManager.h>

namespace MatGui
{

class ArrayDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    ArrayDelegate(Materials::MaterialValue::ValueType type = Materials::MaterialValue::None,
                  const QString& units = QString(),
                  QObject* parent = nullptr);
    virtual ~ArrayDelegate() = default;

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& styleOption,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

private:
    Materials::MaterialValue::ValueType _type;
    QString _units;

    QWidget* createWidget(QWidget* parent, const QVariant& item) const;
};

}  // namespace MatGui

#endif  // MATGUI_ArrayDelegate_H
