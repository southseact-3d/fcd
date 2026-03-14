#ifndef PARTGUI_DLGPARTBOXIMP_H
#define PARTGUI_DLGPARTBOXIMP_H

#include <Gui/InputVector.h>

namespace PartGui
{
class Ui_DlgPartBox;
using Ui_DlgPartBoxPtr = std::shared_ptr<Ui_DlgPartBox>;

class DlgPartBoxImp: public Gui::LocationDialogUiImp
{
    Q_OBJECT

public:
    explicit DlgPartBoxImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgPartBoxImp() override;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGPARTBOXIMP_H
