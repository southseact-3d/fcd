#ifndef MESHGUI_DLGREGULARSOLID_IMP_H
#define MESHGUI_DLGREGULARSOLID_IMP_H

#include <Mod/Mesh/MeshGlobal.h>

#include <QDialog>
#include <memory>

namespace MeshGui
{
class Ui_DlgRegularSolid;
class DlgRegularSolidImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgRegularSolidImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgRegularSolidImp() override;

private:
    void onCreateSolidButtonClicked();

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgRegularSolid> ui;

    Q_DISABLE_COPY_MOVE(DlgRegularSolidImp)
};

}  // namespace MeshGui

#endif  // MESHGUI_DLGREGULARSOLID_IMP_H
