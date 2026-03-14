#ifndef MATGUI_LISTDELEGATE_H
#define MATGUI_LISTDELEGATE_H

#include <QDialog>
#include <QDir>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QSvgWidget>
#include <QTreeView>

#include <Mod/Material/App/MaterialManager.h>
#include <Mod/Material/App/Materials.h>
#include <Mod/Material/App/ModelManager.h>

#include "BaseDelegate.h"

namespace MatGui
{

class ListDelegate: public BaseDelegate
{
    Q_OBJECT
public:
    ListDelegate(Materials::MaterialValue::ValueType type = Materials::MaterialValue::None,
                 const QString& units = QString(),
                 QObject* parent = nullptr);
    virtual ~ListDelegate() = default;

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

protected:
    Materials::MaterialValue::ValueType getType(const QModelIndex& index) const override
    {
        Q_UNUSED(index)
        return _type;
    }
    QString getUnits(const QModelIndex& index) const override
    {
        Q_UNUSED(index)
        return _units;
    }
    QVariant getValue(const QModelIndex& index) const override;
    void setValue(QAbstractItemModel* model,
                  const QModelIndex& index,
                  const QVariant& value) const override;
    void notifyChanged(const QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    Materials::MaterialValue::ValueType _type;
    QString _units;
};

}  // namespace MatGui

#endif  // MATGUI_LISTDELEGATE_H
