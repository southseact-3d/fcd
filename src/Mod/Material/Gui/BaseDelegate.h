#ifndef MATGUI_BASEDELEGATE_H
#define MATGUI_BASEDELEGATE_H

#include <QDialog>
#include <QDir>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QSvgWidget>
#include <QTreeView>

#include <Base/Color.h>

#include <Mod/Material/App/MaterialManager.h>
#include <Mod/Material/App/Materials.h>
#include <Mod/Material/App/ModelManager.h>

namespace MatGui
{

using Base::Color;

class BaseDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    BaseDelegate(QObject* parent = nullptr);
    virtual ~BaseDelegate() = default;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& styleOption,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

    // Q_SIGNALS:
    /** Emits this signal when a property has changed */
    // void propertyChange(const QModelIndex& index, const QString value);

protected:
    virtual Materials::MaterialValue::ValueType getType(const QModelIndex& index) const = 0;
    virtual QString getUnits(const QModelIndex& index) const = 0;
    virtual QVariant getValue(const QModelIndex& index) const = 0;
    virtual void
    setValue(QAbstractItemModel* model, const QModelIndex& index, const QVariant& value) const = 0;
    virtual void notifyChanged(const QAbstractItemModel* model, const QModelIndex& index) const = 0;

    QString getStringValue(const QModelIndex& index) const;
    Color parseColor(const QString& color) const;

    void paintQuantity(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;
    void paintImage(QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;
    void
    paintSVG(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paintColor(QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;
    void paintList(QPainter* painter,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
    void paintMultiLineString(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const;
    void paintArray(QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;

    virtual bool newRow(const QAbstractItemModel* model, const QModelIndex& index) const;
    QWidget* createWidget(QWidget* parent, const QVariant& item, const QModelIndex& index) const;
};

}  // namespace MatGui

#endif  // MATGUI_BASEDELEGATE_H
