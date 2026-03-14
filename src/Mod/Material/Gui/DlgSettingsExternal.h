#ifndef MATGUI_DLGSETTINGSEXTERNAL_H
#define MATGUI_DLGSETTINGSEXTERNAL_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace MatGui
{
class Ui_DlgSettingsExternal;

class DlgSettingsExternal: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsExternal(QWidget* parent = nullptr);
    ~DlgSettingsExternal() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void loadInterfaces();
    void changeEvent(QEvent* e) override;

    std::string getPreferences() const;
    std::string getPreferencesInterfaces() const;

private:
    QString toPerCent(double value) const;

    std::unique_ptr<Ui_DlgSettingsExternal> ui;
};

}  // namespace MatGui

#endif  // MATGUI_DLGSETTINGSDATABASE_H
