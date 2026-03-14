#ifndef GUI_DIALOG_DLGONLINEHELP_IMP_H
#define GUI_DIALOG_DLGONLINEHELP_IMP_H

#include "PropertyPage.h"
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgOnlineHelp;

/** This class implements the dialog for downloading the online documentation.
 * Moreover it allows one to specify location of the start page an an external browser.
 * Here you can specify to use a proxy if necessary and some more stuff.
 * \author Werner Mayer
 */
class DlgOnlineHelpImp: public PreferencePage
{
    Q_OBJECT

public:
    DlgOnlineHelpImp(QWidget* parent = nullptr);
    ~DlgOnlineHelpImp() override;

    static QString getStartpage();

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

protected:
    void onLineEditDownloadFileNameSelected(const QString&);

private:
    std::unique_ptr<Ui_DlgOnlineHelp> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGONLINEHELP_IMP_H
