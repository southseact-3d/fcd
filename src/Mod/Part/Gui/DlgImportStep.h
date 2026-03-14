#ifndef PARTGUI_DLGIMPORTSTEP_H
#define PARTGUI_DLGIMPORTSTEP_H

#include <Mod/Part/PartGlobal.h>
#include <Gui/PropertyPage.h>
#include <QDialog>

class QButtonGroup;
class QCheckBox;

namespace PartGui
{

struct StepImportSettings
{
    bool merge = false;
    bool useLinkGroup = false;
    bool useBaseName = true;
    bool importHidden = true;
    bool reduceObjects = false;
    bool showProgress = false;
    bool expandCompound = false;
    int mode = 0;
    int codePage = -1;
};

class Ui_DlgImportStep;
class DlgImportStep: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit DlgImportStep(QWidget* parent = nullptr);
    ~DlgImportStep() override;

    void saveSettings() override;
    void loadSettings() override;

    StepImportSettings getSettings() const;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_DlgImportStep> ui;
};

// ----------------------------------------------------------------------------

class PartGuiExport TaskImportStep: public QDialog
{
    Q_OBJECT

public:
    explicit TaskImportStep(QWidget* parent = nullptr);
    ~TaskImportStep() override;

    bool showDialog() const;
    void accept() override;
    StepImportSettings getSettings() const;

private:
    QCheckBox* showThis;
    std::unique_ptr<DlgImportStep> ui;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGIMPORTSTEP_H
