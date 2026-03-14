#ifndef DrawViewSpreadsheet_h_
#define DrawViewSpreadsheet_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewSymbol.h"


namespace TechDraw
{

class TechDrawExport DrawViewSpreadsheet : public TechDraw::DrawViewSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewSpreadsheet);

public:
    DrawViewSpreadsheet();
    ~DrawViewSpreadsheet() override;
    App::PropertyLink         Source;
    App::PropertyString       CellStart;
    App::PropertyString       CellEnd;
    App::PropertyFont         Font;
    App::PropertyColor        TextColor;
    App::PropertyFloat        LineWidth;
    App::PropertyFloat        TextSize;

    App::PropertyLink         Owner;

    App::DocumentObjectExecReturn *execute() override;
    short mustExecute() const override;

    App::PropertyLink *getOwnerProperty() override { return &Owner; }

    std::string getSheetImage();

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderSpreadsheet";
    }

protected:
    void onChanged(const App::Property* prop) override;
    std::vector<std::string> getAvailColumns();
    std::string getSVGHead();
    std::string getSVGTail();
    int colInList(const std::vector<std::string>& list,
                   const std::string& toFind);

private:
};

using DrawViewSpreadsheetPython = App::FeaturePythonT<DrawViewSpreadsheet>;


} //namespace TechDraw


#endif
