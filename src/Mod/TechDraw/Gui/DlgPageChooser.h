#ifndef GUI_DLGPAGECHOOSER_H
#define GUI_DLGPAGECHOOSER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QDialog>

namespace TechDrawGui {

class Ui_DlgPageChooser;

//NOLINTBEGIN
class TechDrawGuiExport DlgPageChooser : public QDialog
{
    Q_OBJECT
//NOLINTEND

public:
    DlgPageChooser(const std::vector<std::string>& labels,
                   const std::vector<std::string>& names,
                   QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgPageChooser() override;

    std::string getSelection() const;
    void accept() override;
    void reject() override;

public Q_SLOTS:
    void slotChangedSelection();

private:
    void fillList(std::vector<std::string> labels, std::vector<std::string> names);

    Ui_DlgPageChooser* ui;
};

} // namespace Gui


#endif // GUI_DLGPAGECHOOSER_H

