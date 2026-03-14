#ifndef GUI_DIALOG_DLGMACROEXECUTEIMP_H
#define GUI_DIALOG_DLGMACROEXECUTEIMP_H

#include <QDialog>
#include <memory>
#include "PythonTracing.h"
#include "Window.h"

class QTreeWidgetItem;

namespace Gui
{
namespace Dialog
{
class Ui_DlgMacroExecute;

/**
 * The DlgMacroExecuteImp class implements a dialog to execute or edit a
 * recorded macro.
 * \author Jürgen Riegel
 */
class DlgMacroExecuteImp: public QDialog, public Gui::WindowParameter
{
    Q_OBJECT

public:
    explicit DlgMacroExecuteImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgMacroExecuteImp() override;

    void accept() override;

private:
    void setupConnections();
    void onFileChooserFileNameChanged(const QString&);
    void onCreateButtonClicked();
    void onDeleteButtonClicked();
    void onEditButtonClicked();
    void onRenameButtonClicked();
    void onDuplicateButtonClicked();
    void onToolbarButtonClicked();
    void onAddonsButtonClicked();
    void onFolderButtonClicked();

    void onUserMacroListBoxCurrentItemChanged(QTreeWidgetItem*);
    void onSystemMacroListBoxCurrentItemChanged(QTreeWidgetItem*);
    void onTabMacroWidgetCurrentChanged(int index);
    void onLineEditFindTextChanged(const QString&);
    void onLineEditFindInFilesTextChanged(const QString&);

protected:
    void fillUpList();
    void fillUpListForDir(const QString& dirPath, bool systemWide);
    QStringList filterFiles(const QString&);

protected:
    QString macroPath;

private:
    std::unique_ptr<PythonTracingWatcher> watcher;
    std::unique_ptr<Ui_DlgMacroExecute> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGMACROEXECUTEIMP_H
