#ifndef GUI_DIALOG_DLGPARAMETERFIND_H
#define GUI_DIALOG_DLGPARAMETERFIND_H

#include <QDialog>
#include <FCGlobal.h>

class QTreeWidgetItem;

namespace Gui
{
namespace Dialog
{

class Ui_DlgParameterFind;
class DlgParameterImp;

class GuiExport DlgParameterFind: public QDialog
{
    Q_OBJECT

public:
    explicit DlgParameterFind(DlgParameterImp* parent);
    ~DlgParameterFind() override;

    void accept() override;
    void reject() override;

private:
    void setupConnections();
    void onLineEditTextChanged(const QString&);
    void onCheckGroupsToggled(bool);
    void onCheckNamesToggled(bool);
    void onCheckValuesToggled(bool);

private:
    struct Options
    {
        QString text;
        bool group = true;
        bool name = true;
        bool value = true;
        bool match = false;
    };
    QTreeWidgetItem* findItem(QTreeWidgetItem* root, const Options& opt) const;
    bool matches(QTreeWidgetItem* item, const Options& opt) const;

private:
    Ui_DlgParameterFind* ui;
    DlgParameterImp* dialog;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGPARAMETERFIND_H
