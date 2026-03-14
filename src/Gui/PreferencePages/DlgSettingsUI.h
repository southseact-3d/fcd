#ifndef GUI_DIALOG_DlgSettingsUI_H
#define GUI_DIALOG_DlgSettingsUI_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{

class PrefComboBox;

namespace Dialog
{
class Ui_DlgSettingsUI;

/**
 * The DlgSettingsUI class implements a preference page to change theme settings.
 * @author Pierre-Louis Boyer
 */
class DlgSettingsUI: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsUI(QWidget* parent = nullptr);
    ~DlgSettingsUI() override;

    void saveSettings() override;
    void loadSettings() override;

    void loadStyleSheet();

protected:
    void changeEvent(QEvent* e) override;

    void populateStylesheets(
        const char* key,
        const char* path,
        PrefComboBox* combo,
        const char* def,
        QStringList filter = QStringList()
    );

    void openThemeEditor();

private:
    std::unique_ptr<Ui_DlgSettingsUI> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DlgSettingsUI_H
