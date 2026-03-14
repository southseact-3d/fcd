#ifndef MeasureGui_DlgPrefsAppearanceImp_H
#define MeasureGui_DlgPrefsAppearanceImp_H

#include <memory>

#include <Gui/PropertyPage.h>
#include <Mod/Measure/MeasureGlobal.h>


namespace MeasureGui
{

class Ui_DlgPrefsMeasureAppearanceImp;

class DlgPrefsMeasureAppearanceImp: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgPrefsMeasureAppearanceImp(QWidget* parent = nullptr);
    ~DlgPrefsMeasureAppearanceImp() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgPrefsMeasureAppearanceImp> ui;
};

}  // namespace MeasureGui

#endif  // MeasureGui_DlgPrefsAppearanceImp_H
