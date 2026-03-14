#ifndef MATGUI_ARRAYMODEL_H
#define MATGUI_ARRAYMODEL_H

#include <memory>

#include <QAbstractTableModel>
#include <QDialog>
#include <QStandardItem>
#include <QTableView>

#include <Mod/Material/App/Materials.h>
#include <Mod/Material/App/Model.h>

namespace MatGui
{

class AbstractArrayModel: public QAbstractTableModel
{
public:
    explicit AbstractArrayModel(QObject* parent = nullptr);
    ~AbstractArrayModel() override = default;

    virtual bool newRow(const QModelIndex& index) const = 0;
};

class Array2DModel: public AbstractArrayModel
{
public:
    explicit Array2DModel(const std::shared_ptr<Materials::MaterialProperty>& property = nullptr,
                          const std::shared_ptr<Materials::Array2D>& value = nullptr,
                          QObject* parent = nullptr);
    ~Array2DModel() override = default;

    // Overridden virtual functions
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool newRow(const QModelIndex& index) const override;
    void deleteRow(const QModelIndex& index);
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Resizing functions
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

private:
    std::shared_ptr<Materials::MaterialProperty> _property;
    std::shared_ptr<Materials::Array2D> _value;
};

class Array3DDepthModel: public AbstractArrayModel
{
public:
    explicit Array3DDepthModel(
        const std::shared_ptr<Materials::MaterialProperty>& property = nullptr,
        const std::shared_ptr<Materials::Array3D>& value = nullptr,
        QObject* parent = nullptr);
    ~Array3DDepthModel() override = default;

    // Overridden virtual functions
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool newRow(const QModelIndex& index) const override;
    void deleteRow(const QModelIndex& index);
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return 1;
    }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Resizing functions
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

private:
    std::shared_ptr<Materials::MaterialProperty> _property;
    std::shared_ptr<Materials::Array3D> _value;
};

class Array3DModel: public AbstractArrayModel
{
public:
    explicit Array3DModel(const std::shared_ptr<Materials::MaterialProperty>& property = nullptr,
                          const std::shared_ptr<Materials::Array3D>& value = nullptr,
                          QObject* parent = nullptr);
    ~Array3DModel() override = default;

    // Overridden virtual functions
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool newRow(const QModelIndex& index) const override;
    void deleteRow(const QModelIndex& index);
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Resizing functions
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

    void updateData();

private:
    std::shared_ptr<Materials::MaterialProperty> _property;
    std::shared_ptr<Materials::Array3D> _value;
};

}  // namespace MatGui

#endif  // MATGUI_ARRAYMODEL_H
