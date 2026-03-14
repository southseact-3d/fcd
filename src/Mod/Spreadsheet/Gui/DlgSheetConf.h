#ifndef DLG_SHEETCONF_H
#define DLG_SHEETCONF_H

#include <Mod/Spreadsheet/App/Sheet.h>
#include <QDialog>

namespace Ui
{
class DlgSheetConf;
}

namespace SpreadsheetGui
{

class DlgSheetConf: public QDialog
{
    Q_OBJECT

public:
    explicit DlgSheetConf(Spreadsheet::Sheet* sheet, App::Range range, QWidget* parent = nullptr);
    ~DlgSheetConf() override;

    void accept() override;

    App::Property* prepare(
        App::CellAddress& from,
        App::CellAddress& to,
        std::string& rangeConf,
        App::ObjectIdentifier& path,
        bool init
    );

public Q_SLOTS:
    void onDiscard();

private:
    Spreadsheet::Sheet* sheet;
    Ui::DlgSheetConf* ui;
};

}  // namespace SpreadsheetGui

#endif  // DLG_SHEETCONF_H
