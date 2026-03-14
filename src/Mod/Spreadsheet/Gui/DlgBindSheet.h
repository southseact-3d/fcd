#ifndef DLG_BINDSHEET_H
#define DLG_BINDSHEET_H

#include <Mod/Spreadsheet/App/Sheet.h>
#include <QDialog>

namespace Ui
{
class DlgBindSheet;
}

namespace SpreadsheetGui
{

class DlgBindSheet: public QDialog
{
    Q_OBJECT

public:
    explicit DlgBindSheet(
        Spreadsheet::Sheet* sheet,
        const std::vector<App::Range>& range,
        QWidget* parent = nullptr
    );
    ~DlgBindSheet() override;

    void accept() override;

public Q_SLOTS:
    void onDiscard();

private:
    Spreadsheet::Sheet* sheet;
    App::Range range;
    Ui::DlgBindSheet* ui;
};

}  // namespace SpreadsheetGui

#endif  // DLG_BINDSHEET_H
