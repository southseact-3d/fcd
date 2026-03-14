#ifndef PARTGUI_DLGPARTCYLINDERIMP_H
#define PARTGUI_DLGPARTCYLINDERIMP_H

#include <Gui/InputVector.h>

namespace PartGui
{
class Ui_DlgPartCylinder;
using Ui_DlgPartCylinderPtr = std::shared_ptr<Ui_DlgPartCylinder>;

class DlgPartCylinderImp: public Gui::LocationDialogUiImp
{
    Q_OBJECT

public:
    explicit DlgPartCylinderImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgPartCylinderImp() override;

    double getRadius() const;
    double getLength() const;

private:
    Ui_DlgPartCylinderPtr getUi() const;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGPARTCYLINDERIMP_H
