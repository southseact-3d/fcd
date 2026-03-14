#ifndef GUI_DIALOG_DLGSETTINGSGENERAL_H
#define GUI_DIALOG_DLGSETTINGSGENERAL_H

#include <Gui/PropertyPage.h>
#include <memory>
#include <string>

class QTabWidget;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsGeneral;
class DlgCreateNewPreferencePackImp;
class DlgPreferencePackManagementImp;
class DlgRevertToBackupConfigImp;

/** This class implements the settings for the application.
 *  You can change window style, size of pixmaps, size of recent file list and so on
 *  \author Werner Mayer
 */
class DlgSettingsGeneral: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsGeneral(QWidget* parent = nullptr);
    ~DlgSettingsGeneral() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

    void saveThemes();
    void loadThemes();

    static void attachObserver();

protected:
    void changeEvent(QEvent* event) override;

protected Q_SLOTS:
    void onLoadPreferencePackClicked(const std::string& packName);
    void recreatePreferencePackMenu();
    void newPreferencePackDialogAccepted();
    void onManagePreferencePacksClicked();
    void onImportConfigClicked();
    void onThemeChanged(int index);
    void onLinkActivated(const QString& link);

public Q_SLOTS:
    void onUnitSystemIndexChanged(int index);

private:
    void saveUnitSystemSettings();
    void saveDockWindowVisibility();
    void loadDockWindowVisibility();
    void setRecentFileSize();
    void saveAsNewPreferencePack();
    void revertToSavedConfig();
    bool setLanguage();  // Returns true if language has been changed
    void setNumberLocale(bool force = false);
    void setDecimalPointConversion(bool on);
    void retranslateUnits();
    int getCurrentIconSize() const;
    void addIconSizes(int current);
    void translateIconSizes();

private:
    int localeIndex;
    bool themeChanged;
    std::unique_ptr<Ui_DlgSettingsGeneral> ui;
    std::unique_ptr<DlgCreateNewPreferencePackImp> newPreferencePackDialog;
    std::unique_ptr<DlgPreferencePackManagementImp> preferencePackManagementDialog;
    std::unique_ptr<DlgRevertToBackupConfigImp> revertToBackupConfigDialog;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSGENERAL_H
