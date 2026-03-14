#ifndef GUI_DIALOG_DLG_REVERT_TO_BACKUP_CONFIG_IMP
#define GUI_DIALOG_DLG_REVERT_TO_BACKUP_CONFIG_IMP

#include <memory>
#include <QDialog>

namespace Gui
{
namespace Dialog
{
class Ui_DlgRevertToBackupConfig;

/** The DlgRevertToBackupConfigImp class
 * \author Chris Hennes
 */
class DlgRevertToBackupConfigImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgRevertToBackupConfigImp(QWidget* parent = nullptr);
    ~DlgRevertToBackupConfigImp() override;

public Q_SLOTS:
    void accept() override;
    void onItemSelectionChanged();

protected:
    void changeEvent(QEvent* e) override;
    void showEvent(QShowEvent* event) override;

private:
    std::unique_ptr<Ui_DlgRevertToBackupConfig> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLG_REVERT_TO_BACKUP_CONFIG_IMP
