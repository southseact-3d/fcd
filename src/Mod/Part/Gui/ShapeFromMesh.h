#ifndef PARTGUI_SHAPEFROMMESH_H
#define PARTGUI_SHAPEFROMMESH_H

#include <QDialog>
#include <memory>

namespace PartGui
{

class Ui_ShapeFromMesh;
class ShapeFromMesh: public QDialog
{
    Q_OBJECT

public:
    explicit ShapeFromMesh(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~ShapeFromMesh() override;

    void accept() override;

private:
    void perform();
    std::unique_ptr<Ui_ShapeFromMesh> ui;
};

}  // namespace PartGui

#endif  // PARTGUI_SHAPEFROMMESH_H
