#ifndef MATGUI_LISTEDIT_H
#define MATGUI_LISTEDIT_H

#include <memory>

#include <QAction>
#include <QDialog>
#include <QList>
#include <QListView>
#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>

#include <Mod/Material/App/Model.h>

#include "ListModel.h"

namespace MatGui
{

class Ui_ListEdit;

class ListEdit: public QDialog
{
    Q_OBJECT

public:
    ListEdit(const QString& propertyName,
             const std::shared_ptr<Materials::Material>& material,
             QWidget* parent = nullptr);
    ~ListEdit() override = default;

    void onDataChanged(const QModelIndex& topLeft,
                       const QModelIndex& bottomRight,
                       const QVector<int>& roles = QVector<int>());
    void defaultValueChanged(const Base::Quantity& value);
    void onDelete(bool checked);
    void onContextMenu(const QPoint& pos);

    void accept() override;
    void reject() override;

private:
    std::unique_ptr<Ui_ListEdit> ui;
    std::shared_ptr<Materials::Material> _material;
    std::shared_ptr<Materials::MaterialProperty> _property;
    QList<QVariant> _value;

    QAction _deleteAction;

    void setDelegates(QListView* list);
    void setupListView();

    bool newRow(const QModelIndex& index);
    int confirmDelete();
    void deleteSelected();
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
};

}  // namespace MatGui

#endif  // MATGUI_LISTEDIT_H
