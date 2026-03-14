#ifndef MATGUI_MATERIALDELEGATE_H
#define MATGUI_MATERIALDELEGATE_H

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

class MaterialDelegate: public BaseDelegate
{
    Q_OBJECT
public:
    explicit MaterialDelegate(QObject* parent = nullptr);
    ~MaterialDelegate() override = default;

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& styleOption,
                          const QModelIndex& index) const override;
    // QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    // void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    // void setModelData(QWidget* editor,
    //                   QAbstractItemModel* model,
    //                   const QModelIndex& index) const override;

protected:
    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

    Materials::MaterialValue::ValueType getType(const QModelIndex& index) const override;
    QString getUnits(const QModelIndex& index) const override;
    QVariant getValue(const QModelIndex& index) const override;
    void setValue(QAbstractItemModel* model,
                  const QModelIndex& index,
                  const QVariant& value) const override;
    void notifyChanged(const QAbstractItemModel* model, const QModelIndex& index) const override;
    bool newRow(const QAbstractItemModel* model, const QModelIndex& index) const override;

Q_SIGNALS:
    /** Emits this signal when a property has changed */
    void propertyChange(const QString& property, const QVariant& value);

private:
    QWidget* createWidget(QWidget* parent, const QVariant& item, const QModelIndex& index) const;
    void showColorModal(const QString& propertyName, QStandardItem* item);
    void showImageModal(const QString& propertyName, QStandardItem* item);
    void showListModal(const QString& propertyName, QStandardItem* item);
    void showMultiLineStringModal(const QString& propertyName, QStandardItem* item);
    void showArray2DModal(const QString& propertyName, QStandardItem* item);
    void showArray3DModal(const QString& propertyName, QStandardItem* item);
};

}  // namespace MatGui

#endif  // MATGUI_MATERIALDELEGATE_H
