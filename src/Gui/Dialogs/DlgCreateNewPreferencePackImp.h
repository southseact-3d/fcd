#ifndef GUI_DIALOG_DLGCREATENEWTHEMEIMP_H
#define GUI_DIALOG_DLGCREATENEWTHEMEIMP_H

#include <memory>
#include <QDialog>
#include <QRegularExpressionValidator>

#include "PreferencePackManager.h"

class QTreeWidgetItem;

namespace Gui
{

namespace Dialog
{

class Ui_DlgCreateNewPreferencePack;

/**
 * \class DlgCreateNewPreferencePackImp
 *
 * A dialog to request a preferencePack name and a set of preferencePack templates.
 *
 * \author Chris Hennes
 */
class GuiExport DlgCreateNewPreferencePackImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgCreateNewPreferencePackImp(QWidget* parent = nullptr);
    ~DlgCreateNewPreferencePackImp() override;

    void setPreferencePackTemplates(
        const std::vector<PreferencePackManager::TemplateFile>& availableTemplates
    );
    void setPreferencePackNames(const std::vector<std::string>& usedNames);

    std::vector<PreferencePackManager::TemplateFile> selectedTemplates() const;
    std::string preferencePackName() const;
    std::string preferencePackDirectory() const;

protected Q_SLOTS:

    void onItemChanged(QTreeWidgetItem* item, int column);

    void onLineEditTextEdited(const QString& text);

    void onBrowseButtonClicked();

    void accept() override;

private:
    std::unique_ptr<Ui_DlgCreateNewPreferencePack> ui;
    std::map<std::string, QTreeWidgetItem*> _groups;
    std::vector<PreferencePackManager::TemplateFile> _templates;
    QRegularExpressionValidator _nameValidator;
    std::vector<std::string> _existingPackNames;
    QString _cfgFileDirectory;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGCREATENEWTHEMEIMP_H
