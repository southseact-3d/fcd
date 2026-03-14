#ifndef GUI_DIALOG_DLGACTIVATEWINDOW_IMP_H
#define GUI_DIALOG_DLGACTIVATEWINDOW_IMP_H

#include <QDialog>

namespace Gui
{
namespace Dialog
{
class Ui_DlgActivateWindow;
/**
 * The DlgActivateWindowImp class provides a dialog to activate the MDI window
 * of the main window you wish. Since there could be a lot of MDI windows in
 * an application you cannot put all of them into the "Windows" popup menu.
 * \author Werner Mayer
 */
class DlgActivateWindowImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgActivateWindowImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgActivateWindowImp() override;

protected:
    void accept() override;

private:
    Ui_DlgActivateWindow* ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGACTIVATEWINDOW_IMP_H
