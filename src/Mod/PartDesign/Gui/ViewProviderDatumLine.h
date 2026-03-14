#ifndef PARTGUI_ViewProviderDatumLine_H
#define PARTGUI_ViewProviderDatumLine_H

#include "ViewProviderDatum.h"

class SoCoordinate3;

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderDatumLine: public PartDesignGui::ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDatumLine);

public:
    /// Constructor
    ViewProviderDatumLine();
    ~ViewProviderDatumLine() override;

    void attach(App::DocumentObject* obj) override;
    void updateData(const App::Property*) override;

    void setExtents(Base::BoundBox3d bbox) override;
    void setExtents(double l);

private:
    SoCoordinate3* pCoords;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDatumLine_H
