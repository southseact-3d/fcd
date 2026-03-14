#ifndef GUI_DIALOG_DLGMACRORECORDIMP_H
#define GUI_DIALOG_DLGMACRORECORDIMP_H

#include <QDialog>
#include <memory>
#include "Window.h"

namespace Gui
{
class MacroManager;
namespace Dialog
{
class Ui_DlgMacroRecord;

/**
 * The DlgMacroRecordImp class implements a dialog to record a macro.
 * \author Jürgen Riegel
 */
class DlgMacroRecordImp: public QDialog, public Gui::WindowParameter
{
    Q_OBJECT

public:
    explicit DlgMacroRecordImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgMacroRecordImp() override;

protected:
    void setupConnections();
    void onButtonStartClicked();
    void onButtonStopClicked();
    void onButtonCloseClicked();
    void onButtonChooseDirClicked();
    void onMacroPathTextChanged(const QString&);

private:
    std::unique_ptr<Ui_DlgMacroRecord> ui;
    /// convenience pointer
    MacroManager* macroManager;
    QString macroPath;  // Macro file to save in
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGMACRORECORDIMP_H
