#ifndef PARTGUI_ViewProviderDatumPoint_H
#define PARTGUI_ViewProviderDatumPoint_H

#include "ViewProviderDatum.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderDatumPoint: public PartDesignGui::ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDatumPoint);

public:
    /// Constructor
    ViewProviderDatumPoint();
    ~ViewProviderDatumPoint() override;

    void attach(App::DocumentObject* obj) override;

    // Note: don't overload setExtents () here because point doesn't really depends on it

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDatumPoint_H
