#ifndef GUI_DIALOG_DLGPROJECTUTILITY_H
#define GUI_DIALOG_DLGPROJECTUTILITY_H

#include <QDialog>
#include <memory>
#include <string>

namespace Gui
{
namespace Dialog
{

class Ui_DlgProjectUtility;
class DlgProjectUtility: public QDialog
{
    Q_OBJECT

public:
    DlgProjectUtility(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgProjectUtility() override;

private:
    void extractButton();
    void createButton();
    void tryExtractArchive(const QString& source, const QString& target);
    void tryCreateArchive(const QString& source, const QString& target, bool openFile);

protected:
    std::unique_ptr<Ui_DlgProjectUtility> ui;
};

}  // namespace Dialog
}  // namespace Gui


#endif  // GUI_DIALOG_DLGPROJECTUTILITY_H
