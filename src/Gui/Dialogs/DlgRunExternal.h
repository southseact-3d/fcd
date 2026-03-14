#ifndef GUI_DIALOG_DlgRunExternal_H
#define GUI_DIALOG_DlgRunExternal_H

#include <memory>
#include <QDialog>
#include <QProcess>
#include <FCGlobal.h>

namespace Gui
{
namespace Dialog
{
class Ui_DlgRunExternal;

/**
 * The DlgRunExternal class implements a dialog to start and control external
 * programs to edit FreeCAD controlled content.
 * \author Jürgen Riegel
 */
class GuiExport DlgRunExternal: public QDialog
{
    Q_OBJECT

public:
    explicit DlgRunExternal(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgRunExternal() override;

    void addArgument(const QString&);
    int runProcess();

protected Q_SLOTS:
    void reject() override;
    void accept() override;
    virtual void abort();
    virtual void advanced();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void onChooseProgramClicked();

private:
    QString ProcName;
    QStringList arguments;
    QProcess process;
    bool advancedHidden;
    std::unique_ptr<Ui_DlgRunExternal> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DlgRunExternal_H
