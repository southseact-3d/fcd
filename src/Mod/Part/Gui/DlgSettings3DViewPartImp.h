#ifndef PARTGUI_DLGSETTINGS3DVIEWIMP_H
#define PARTGUI_DLGSETTINGS3DVIEWIMP_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace PartGui
{
class Ui_DlgSettings3DViewPart;
/**
 * The DlgSettings3DViewPartImp class implements a preference page to change settings
 * for the Inventor viewer.
 * \author Jürgen Riegel
 */
class DlgSettings3DViewPart: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettings3DViewPart(QWidget* parent = nullptr);
    ~DlgSettings3DViewPart() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    void onMaxDeviationValueChanged(double);
    void onMaxAngularDeflectionValueChanged(double);

private:
    std::unique_ptr<Ui_DlgSettings3DViewPart> ui;
    bool checkValue;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGSETTINGS3DVIEWIMP_H
