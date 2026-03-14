#ifndef PARTGUI_DLGSETTINGSGENERAL_H
#define PARTGUI_DLGSETTINGSGENERAL_H

#include <Gui/PropertyPage.h>

class QButtonGroup;

namespace PartGui
{

class Ui_DlgSettingsGeneral;
class DlgSettingsGeneral: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsGeneral(QWidget* parent = nullptr);
    ~DlgSettingsGeneral() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgSettingsGeneral> ui;
};

class Ui_DlgImportExportIges;
class DlgImportExportIges: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgImportExportIges(QWidget* parent = nullptr);
    ~DlgImportExportIges() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgImportExportIges> ui;
    QButtonGroup* bg;
};

class DlgExportStep;
class DlgImportStep;
class DlgExportHeaderStep;
class DlgImportExportStep: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgImportExportStep(QWidget* parent = nullptr);
    ~DlgImportExportStep() override;

protected:
    void saveSettings() override;
    void loadSettings() override;
    void changeEvent(QEvent* e) override;

private:
    DlgExportStep* exportStep;
    DlgImportStep* importStep;
    DlgExportHeaderStep* headerStep;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGSETTINGSGENERAL_H
