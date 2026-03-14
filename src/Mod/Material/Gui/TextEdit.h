#ifndef MATGUI_TEXTEDIT_H
#define MATGUI_TEXTEDIT_H

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

class Ui_TextEdit;

class TextEdit: public QDialog
{
    Q_OBJECT

public:
    TextEdit(const QString& propertyName,
             const std::shared_ptr<Materials::Material>& material,
             QWidget* parent = nullptr);
    ~TextEdit() override = default;

    void accept() override;
    void reject() override;

private:
    std::unique_ptr<Ui_TextEdit> ui;
    std::shared_ptr<Materials::Material> _material;
    std::shared_ptr<Materials::MaterialProperty> _property;
    QString _value;
};

}  // namespace MatGui

#endif  // MATGUI_TEXTEDIT_H
