#ifndef SPREADSHEETGUI_DLGSETTINGSIMP_H
#define SPREADSHEETGUI_DLGSETTINGSIMP_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace SpreadsheetGui
{
class Ui_DlgSettings;

/**
 * The DlgSettingsImp class implements a preference page to change settings
 * for the Spreadsheet workbench.
 * /author TheMarkster, based on work by Jürgen Riegel
 */
class DlgSettingsImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsImp(QWidget* parent = nullptr);
    ~DlgSettingsImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettings> ui;
};

}  // namespace SpreadsheetGui

#endif  // SPREADSHEETGUI_DLGSETTINGSIMP_H
