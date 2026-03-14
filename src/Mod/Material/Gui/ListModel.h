#ifndef MATGUI_LISTMODEL_H
#define MATGUI_LISTMODEL_H

#include <memory>

#include <QAbstractListModel>
#include <QDialog>
#include <QList>
#include <QStandardItem>
#include <QVariant>

#include <Mod/Material/App/Materials.h>
#include <Mod/Material/App/Model.h>

namespace MatGui
{

class ListModel: public QAbstractListModel
{
public:
    ListModel();
    ListModel(std::shared_ptr<Materials::MaterialProperty> property,
              QList<QVariant>& value,
              QObject* parent = nullptr);
    ~ListModel() override = default;

    // Overridden virtual functions
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool newRow(const QModelIndex& index) const;
    void deleteRow(const QModelIndex& index);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Resizing functions
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

private:
    std::shared_ptr<Materials::MaterialProperty> _property;
    QList<QVariant>* _valuePtr;
};

}  // namespace MatGui

#endif  // MATGUI_LISTMODEL_H
