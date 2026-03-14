#ifndef MESHGUI_DLGSETTINGSIMPORTEXPORTIMP_H
#define MESHGUI_DLGSETTINGSIMPORTEXPORTIMP_H

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif
#include <Gui/PropertyPage.h>


namespace MeshGui
{

class Ui_DlgSettingsImportExport;
/**
 * The DlgSettingsImportExportImp class implements a preference page to change settings
 * for Importing and Exporting mesh objects.
 */
class DlgSettingsImportExport: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsImportExport(QWidget* parent = nullptr);
    ~DlgSettingsImportExport() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    Ui_DlgSettingsImportExport* ui;

    Q_DISABLE_COPY_MOVE(DlgSettingsImportExport)
};  // end class DlgSettingsImportExport

}  // namespace MeshGui

#endif  // MESHGUI_DLGSETTINGSIMPORTEXPORTIMP_H
