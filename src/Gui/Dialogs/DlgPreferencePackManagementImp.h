#ifndef GUI_DIALOG_DLGPREFERENCEPACKMANAGEMENTIMP_H
#define GUI_DIALOG_DLGPREFERENCEPACKMANAGEMENTIMP_H

#include <memory>
#include <QDialog>
#include <filesystem>
#include <FCGlobal.h>


class QTreeWidgetItem;

namespace Gui
{

namespace Dialog
{

class Ui_DlgPreferencePackManagement;

/**
 * \class DlgCreateNewPreferencePackImp
 *
 * A dialog to request a preferencePack name and a set of preferencePack templates.
 *
 * \author Chris Hennes
 */
class GuiExport DlgPreferencePackManagementImp: public QDialog
{
    Q_OBJECT

public:
    DlgPreferencePackManagementImp(QWidget* parent = nullptr);
    ~DlgPreferencePackManagementImp() override;

Q_SIGNALS:
    void packVisibilityChanged();

protected Q_SLOTS:

    void deleteUserPack(const std::string& prefPackName);
    void hideBuiltInPack(const std::string& prefPackName);
    void hideInstalledPack(const std::string& addonName, const std::string& prefPackName);
    void showAddonManager();

private:
    enum class TreeWidgetType
    {
        BUILTIN,
        USER,
        ADDON
    };

    std::unique_ptr<Ui_DlgPreferencePackManagement> ui;

    void addTreeNode(
        const std::string& name,
        const std::vector<std::string>& contents,
        TreeWidgetType twt
    );
    void updateTree();
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGPREFERENCEPACKMANAGEMENTIMP_H
