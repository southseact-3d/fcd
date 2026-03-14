#ifndef STARTGUI_DLGSTARTPREFERENCESIMP_H
#define STARTGUI_DLGSTARTPREFERENCESIMP_H

#include <memory>
#include <Gui/PropertyPage.h>


class Ui_DlgStartPreferences;
namespace StartGui
{
class DlgStartPreferencesImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgStartPreferencesImp(QWidget* parent = nullptr);
    ~DlgStartPreferencesImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgStartPreferences> ui;
};

}  // namespace StartGui

#endif  // STARTGUI_DLGSTARTPREFERENCESIMP_H
