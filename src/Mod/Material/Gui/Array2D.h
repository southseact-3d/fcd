#ifndef MATGUI_ARRAY2D_H
#define MATGUI_ARRAY2D_H

#include <memory>

#include <QAbstractTableModel>
#include <QAction>
#include <QDialog>
#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>

#include <Mod/Material/App/Model.h>

#include "ArrayModel.h"

namespace MatGui
{

class Ui_Array2D;

class Array2D: public QDialog
{
    Q_OBJECT

public:
    Array2D(const QString& propertyName,
            const std::shared_ptr<Materials::Material>& material,
            QWidget* parent = nullptr);
    ~Array2D() override = default;

    void onDataChanged(const QModelIndex& topLeft,
                       const QModelIndex& bottomRight,
                       const QVector<int>& roles = QVector<int>());
    void onDelete(bool checked);
    void onContextMenu(const QPoint& pos);

    void accept() override;
    void reject() override;

private:
    std::unique_ptr<Ui_Array2D> ui;
    std::shared_ptr<Materials::Material> _material;
    std::shared_ptr<Materials::MaterialProperty> _property;
    std::shared_ptr<Materials::Array2D> _value;

    QAction _deleteAction;

    void setColumnWidths(QTableView* table);
    void setColumnDelegates(QTableView* table);
    void setupArray();

    bool newRow(const QModelIndex& index);
    int confirmDelete();
    void deleteSelected();
};

}  // namespace MatGui

#endif  // MATGUI_ARRAY2D_H
