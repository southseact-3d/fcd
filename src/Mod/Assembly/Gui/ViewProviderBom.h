#ifndef ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBom_H
#define ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBom_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <Mod/Spreadsheet/Gui/ViewProviderSpreadsheet.h>


namespace AssemblyGui
{

class AssemblyGuiExport ViewProviderBom: public SpreadsheetGui::ViewProviderSheet
{
    PROPERTY_HEADER_WITH_OVERRIDE(AssemblyGui::ViewProviderBom);


public:
    ViewProviderBom();
    ~ViewProviderBom() override;

    QIcon getIcon() const override;

    bool doubleClicked() override;
};

}  // namespace AssemblyGui

#endif  // ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBom_H
