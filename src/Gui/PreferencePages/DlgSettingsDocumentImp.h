#ifndef GUI_DIALOG_DLGSETTINGSDOCUMENT_IMP_H
#define GUI_DIALOG_DLGSETTINGSDOCUMENT_IMP_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsDocument;

/**
 * The DlgSettingsDocumentImp class implements a preference page to change settings
 * for the document.
 * \author Jürgen Riegel
 */
class DlgSettingsDocumentImp: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsDocumentImp(QWidget* parent = nullptr);
    ~DlgSettingsDocumentImp() override;

    void saveSettings() override;
    void loadSettings() override;

protected Q_SLOTS:
    void onLicenseTypeChanged(int index);

protected:
    void changeEvent(QEvent* e) override;
    void addLicenseTypes();

private:
    std::unique_ptr<Ui_DlgSettingsDocument> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSDOCUMENT_IMP_H
