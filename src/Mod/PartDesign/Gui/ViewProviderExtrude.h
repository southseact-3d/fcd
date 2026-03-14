#ifndef PARTGUI_ViewProviderExtrude_H
#define PARTGUI_ViewProviderExtrude_H

#include "ViewProviderSketchBased.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderExtrude: public ViewProviderSketchBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderExtrude);

public:
    ViewProviderExtrude() = default;
    ~ViewProviderExtrude() override = default;

    void highlightShapeFaces(const std::vector<std::string>& faces);
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderExtrude_H
