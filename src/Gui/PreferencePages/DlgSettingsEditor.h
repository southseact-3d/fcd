#ifndef GUI_DIALOG_DLGSETTINGSEDITOR_H
#define GUI_DIALOG_DLGSETTINGSEDITOR_H

#include <Gui/PropertyPage.h>
#include <memory>

class QTreeWidgetItem;

namespace Gui
{
class PythonSyntaxHighlighter;

namespace Dialog
{
class Ui_DlgSettingsEditor;

/** This class implements a preferences page for the editor settings.
 *  Here you can change different color settings and font for editors.
 *  @author Werner Mayer
 */
struct DlgSettingsEditorP;
class DlgSettingsEditor: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsEditor(QWidget* parent = nullptr);
    ~DlgSettingsEditor() override;

public:
    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

private:
    void setupConnections();
    void onDisplayItemsCurrentItemChanged(QTreeWidgetItem* i);
    void onColorButtonChanged();
    void onFontFamilyActivated(const QString&);
    void onFontSizeValueChanged(const QString&);

protected:
    void changeEvent(QEvent* e) override;
    void setEditorTabWidth(int);

private:
    std::unique_ptr<Ui_DlgSettingsEditor> ui;
    DlgSettingsEditorP* d;
    Gui::PythonSyntaxHighlighter* pythonSyntax;

    DlgSettingsEditor(const DlgSettingsEditor&);
    DlgSettingsEditor& operator=(const DlgSettingsEditor&);
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSEDITOR_H
