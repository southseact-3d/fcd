#ifndef PARTGUI_ViewProviderDatumCoordinateSystem_H
#define PARTGUI_ViewProviderDatumCoordinateSystem_H

#include "ViewProviderDatum.h"

class SoCoordinate3;
class SoFont;
class SoTranslation;

namespace Gui
{
class SoAutoZoomTranslation;
}

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderDatumCoordinateSystem: public PartDesignGui::ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDatumCoordinateSystem);

public:
    App::PropertyFloatConstraint Zoom;
    App::PropertyIntegerConstraint FontSize;
    App::PropertyBool ShowLabel;

    /// Constructor
    ViewProviderDatumCoordinateSystem();
    ~ViewProviderDatumCoordinateSystem() override;

    void attach(App::DocumentObject* obj) override;
    void updateData(const App::Property*) override;
    void onChanged(const App::Property*) override;

    void setExtents(Base::BoundBox3d bbox) override;

    SoDetail* getDetail(const char* subelement) const override;
    std::string getElement(const SoDetail* detail) const override;

private:
    void setupLabels();

private:
    SoCoordinate3* coord;
    SoTranslation* axisLabelXTrans;
    SoTranslation* axisLabelXToYTrans;
    SoTranslation* axisLabelYToZTrans;
    SoFont* font;
    SoSwitch* labelSwitch;
    Gui::SoAutoZoomTranslation* autoZoom;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDatumCoordinateSystem_H
