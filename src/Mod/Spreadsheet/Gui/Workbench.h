#ifndef SPREADSHEET_WORKBENCH_H
#define SPREADSHEET_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/Spreadsheet/SpreadsheetGlobal.h>

class QtColorPicker;
class QColor;

namespace SpreadsheetGui
{

/**
 * @author Eivind Kvedalen
 */

class SpreadsheetGuiExport WorkbenchHelper: public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void setForegroundColor(const QColor& color);
    void setBackgroundColor(const QColor& color);
};

class SpreadsheetGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;
    void activated() override;

private:
    bool initialized;
    std::unique_ptr<WorkbenchHelper> workbenchHelper;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace SpreadsheetGui


#endif  // SPREADSHEET_WORKBENCH_H
