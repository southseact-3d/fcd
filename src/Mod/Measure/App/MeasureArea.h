#ifndef MEASURE_MEASUREAREA_H
#define MEASURE_MEASUREAREA_H

#include <Mod/Measure/MeasureGlobal.h>

#include <string>

#include <App/PropertyLinks.h>
#include <App/PropertyUnits.h>
#include <App/GeoFeature.h>

#include <Mod/Part/App/MeasureInfo.h>

#include "MeasureBase.h"


namespace Measure
{


class MeasureExport MeasureArea: public Measure::MeasureBaseExtendable<Part::MeasureAreaInfo>
{
    PROPERTY_HEADER_WITH_OVERRIDE(Measure::MeasureArea);

public:
    /// Constructor
    MeasureArea();
    ~MeasureArea() override;

    App::PropertyLinkSubList Elements;
    App::PropertyArea Area;

    App::DocumentObjectExecReturn* execute() override;

    const char* getViewProviderName() const override
    {
        return "MeasureGui::ViewProviderMeasureArea";
    }

    static bool isValidSelection(const App::MeasureSelection& selection);
    void parseSelection(const App::MeasureSelection& selection) override;

    std::vector<std::string> getInputProps() override
    {
        return {"Elements"};
    }
    App::Property* getResultProp() override
    {
        return &this->Area;
    }

    // Return a placement for the viewprovider, just use the first element for now
    Base::Placement getPlacement() const override;

    // Return the object we are measuring
    std::vector<App::DocumentObject*> getSubject() const override;


private:
    static bool isSupported(App::MeasureElementType type);
    void onChanged(const App::Property* prop) override;
};

}  // namespace Measure


#endif  // MEASURE_MEASUREAREA_H
