#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <Mod/Spreadsheet/App/Sheet.h>
#include <QDialog>

namespace Ui
{
class PropertiesDialog;
}

namespace SpreadsheetGui
{

class PropertiesDialog: public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesDialog(
        Spreadsheet::Sheet* _sheet,
        const std::vector<App::Range>& _ranges,
        QWidget* parent = nullptr
    );
    ~PropertiesDialog() override;

    void apply();
    void selectAlias();

private Q_SLOTS:
    void foregroundColorChanged(const QColor& color);
    void backgroundColorChanged(const QColor& color);
    void alignmentChanged();
    void styleChanged();
    void displayUnitChanged(const QString& text);
    void aliasChanged(const QString& text);

private:
    Spreadsheet::Sheet* sheet;
    std::vector<App::Range> ranges;
    Ui::PropertiesDialog* ui;
    Base::Color foregroundColor;
    Base::Color backgroundColor;
    int alignment;
    std::set<std::string> style;
    Spreadsheet::DisplayUnit displayUnit;
    std::string alias;

    Base::Color orgForegroundColor;
    Base::Color orgBackgroundColor;
    int orgAlignment;
    std::set<std::string> orgStyle;
    Spreadsheet::DisplayUnit orgDisplayUnit;
    std::string orgAlias;

    bool displayUnitOk;
    bool aliasOk;
};

}  // namespace SpreadsheetGui

#endif  // PROPERTIESDIALOG_H
