#ifndef GUI_DIALOG_DLGACTIONS_IMP_H
#define GUI_DIALOG_DLGACTIONS_IMP_H

#include <memory>
#include "PropertyPage.h"
#include <QDialog>
#include <QList>
#include <QPair>

class QListWidgetItem;
class QTreeWidgetItem;
class QLabel;
class QGridLayout;
class QLineEdit;

namespace Gui
{
namespace Dialog
{

class Ui_DlgCustomActions;

/** This class implements the creation of user defined actions executing a recorded macro.
 * It is possible to use these actions in own toolbars or commandbars.
 * @see Gui::Action
 * @see MacroCommand
 * @see Command
 * \author Werner Mayer
 */
class DlgCustomActionsImp: public CustomizeActionPage
{
    Q_OBJECT

public:
    explicit DlgCustomActionsImp(QWidget* parent = nullptr);
    ~DlgCustomActionsImp() override;

Q_SIGNALS:
    void addMacroAction(const QByteArray&);
    void removeMacroAction(const QByteArray&);
    void modifyMacroAction(const QByteArray&);

protected:
    /** Trigger for reparent event. */
    bool event(QEvent* e) override;
    void changeEvent(QEvent* e) override;

protected:
    void setupConnections();
    /** Enables/disables buttons for deletion */
    void onActionListWidgetItemActivated(QTreeWidgetItem* i);
    /** Opens a iconview to select a pixmap */
    void onButtonChoosePixmapClicked();
    /** Adds a custom action */
    void onButtonAddActionClicked();
    /** Deletes a custom action */
    void onButtonRemoveActionClicked();
    /** Shows the setup of the action */
    void onButtonReplaceActionClicked();

protected Q_SLOTS:
    void onAddMacroAction(const QByteArray&) override;
    void onRemoveMacroAction(const QByteArray&) override;
    void onModifyMacroAction(const QByteArray&) override;

private:
    /** Shows all actions and their pixmaps if available  */
    void showActions();

private:
    std::unique_ptr<Ui_DlgCustomActions> ui;
    QString m_sPixmap; /**< Name of the specified pixmap */
    bool bChanged = false;
};

class Ui_DlgChooseIcon;
class IconDialog: public QDialog
{
    Q_OBJECT

public:
    explicit IconDialog(QWidget* parent);
    ~IconDialog() override;
    void resizeEvent(QResizeEvent*) override;
    QListWidgetItem* currentItem() const;

private:
    void onAddIconPath();

private:
    Ui_DlgChooseIcon* ui;
};

class IconFolders: public QDialog
{
    Q_OBJECT

public:
    IconFolders(const QStringList&, QWidget* parent);
    ~IconFolders() override;
    QStringList getPaths() const;

private:
    void addFolder();
    void removeFolder();

private:
    bool restart;
    int maxLines;
    QGridLayout* gridLayout;
    QLabel* textLabel;
    QPushButton* addButton;
    QList<QPair<QLineEdit*, QPushButton*>> buttonMap;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGACTIONS_IMP_H
