#ifndef MATGUI_IMAGEEDIT_H
#define MATGUI_IMAGEEDIT_H

#include <memory>

#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QList>
#include <QListView>
#include <QPoint>
#include <QResizeEvent>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVariant>

#include <Mod/Material/App/Model.h>

#include "ListModel.h"

namespace MatGui
{

class Ui_ImageEdit;

class ImageLabel: public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget* parent = 0);
    ~ImageLabel() = default;

    void setPixmap(const QPixmap& pixmap);
    void setSVG(const QString& svg);
    void renderSVG();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    QPixmap _pixmap;
    QString _svg;
};

class ImageEdit: public QDialog
{
    Q_OBJECT

public:
    ImageEdit(const QString& propertyName,
              const std::shared_ptr<Materials::Material>& material,
              QWidget* parent = nullptr);
    ~ImageEdit() override = default;

    void accept() override;
    void reject() override;

private Q_SLOTS:
    void onFileSelect(bool checked);

private:
    std::unique_ptr<Ui_ImageEdit> ui;
    std::shared_ptr<Materials::Material> _material;
    std::shared_ptr<Materials::MaterialProperty> _property;

    QPixmap _pixmap;
    QString _svg;

    void showPixmap();
    void showSVG();

    QString selectFile(const QString& filePatterns);
    void onFileSelectImage();
    void onFileSelectSVG();
};

}  // namespace MatGui

#endif  // MATGUI_IMAGEEDIT_H
