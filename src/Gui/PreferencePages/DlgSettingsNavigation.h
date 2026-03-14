#ifndef GUI_DIALOG_DLGSETTINGSNAVIGATION_H
#define GUI_DIALOG_DLGSETTINGSNAVIGATION_H

#include <memory>
#include "PropertyPage.h"
#include <QDialog>

class QDoubleSpinBox;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsNavigation;

/**
 * The Ui_DlgSettingsNavigation class implements a preference page to change settings
 * for the Inventor viewer.
 * \author Werner Mayer
 */
class DlgSettingsNavigation: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsNavigation(QWidget* parent = nullptr);
    ~DlgSettingsNavigation() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

private:
    void onMouseButtonClicked();
    void onNewDocViewChanged(int);

protected:
    void changeEvent(QEvent* e) override;
    void retranslate();
    void addOrientations();
    void translateOrientations();

private:
    std::unique_ptr<Ui_DlgSettingsNavigation> ui;
    double q0, q1, q2, q3;
};

class CameraDialog: public QDialog
{
    Q_OBJECT

public:
    explicit CameraDialog(QWidget* parent = nullptr);
    ~CameraDialog() override;
    void setValues(double q0, double q1, double q2, double q3);
    void getValues(double& q0, double& q1, double& q2, double& q3) const;


private:
    void onCurrentViewClicked();

private:
    QDoubleSpinBox* sb0;
    QDoubleSpinBox* sb1;
    QDoubleSpinBox* sb2;
    QDoubleSpinBox* sb3;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSNAVIGATION_H
