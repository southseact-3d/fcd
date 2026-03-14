#ifndef PARTGUI_ViewProviderDatumPlane_H
#define PARTGUI_ViewProviderDatumPlane_H

#include "ViewProviderDatum.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderDatumPlane: public PartDesignGui::ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDatumPlane);

public:
    /// Constructor
    ViewProviderDatumPlane();
    ~ViewProviderDatumPlane() override;

    void attach(App::DocumentObject* obj) override;
    void updateData(const App::Property*) override;

    void setExtents(Base::BoundBox3d bbox) override;
    void setExtents(double l, double w);

private:
    SoCoordinate3* pCoords;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDatumPlane_H
