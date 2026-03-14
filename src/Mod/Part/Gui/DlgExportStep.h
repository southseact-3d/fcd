#ifndef PARTGUI_DLGEXPORTSTEP_H
#define PARTGUI_DLGEXPORTSTEP_H

#include <Mod/Part/PartGlobal.h>
#include <Gui/PropertyPage.h>
#include <QDialog>

class QButtonGroup;
class QCheckBox;

namespace PartGui
{

struct StepSettings
{
    bool exportLegacy = false;
    bool exportHidden = false;
    bool keepPlacement = false;
};

// ----------------------------------------------------------------------------

class Ui_DlgExportStep;
class DlgExportStep: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgExportStep(QWidget* parent = nullptr);
    ~DlgExportStep() override;

    void saveSettings() override;
    void loadSettings() override;

    StepSettings getSettings() const;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgExportStep> ui;
};

// ----------------------------------------------------------------------------

class Ui_DlgExportHeaderStep;
class DlgExportHeaderStep: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgExportHeaderStep(QWidget* parent = nullptr);
    ~DlgExportHeaderStep() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgExportHeaderStep> ui;
};

// ----------------------------------------------------------------------------

class PartGuiExport TaskExportStep: public QDialog
{
    Q_OBJECT

public:
    explicit TaskExportStep(QWidget* parent = nullptr);
    ~TaskExportStep() override;

    bool showDialog() const;
    void accept() override;
    StepSettings getSettings() const;

private:
    QCheckBox* showThis;
    std::unique_ptr<DlgExportStep> ui;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGEXPORTSTEP_H
