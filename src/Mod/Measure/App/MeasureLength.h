#ifndef MEASURE_MEASURELENGTH_H
#define MEASURE_MEASURELENGTH_H

#include <Mod/Measure/MeasureGlobal.h>

#include <App/PropertyLinks.h>
#include <App/PropertyUnits.h>
#include <App/GeoFeature.h>

#include <Mod/Part/App/MeasureInfo.h>
#include <Mod/Part/App/TopoShape.h>

#include "MeasureBase.h"

namespace Measure
{

class MeasureExport MeasureLength: public Measure::MeasureBaseExtendable<Part::MeasureLengthInfo>
{
    PROPERTY_HEADER_WITH_OVERRIDE(Measure::MeasureLength);

public:
    /// Constructor
    MeasureLength();
    ~MeasureLength() override;

    App::PropertyLinkSubList Elements;
    App::PropertyDistance Length;

    App::DocumentObjectExecReturn* execute() override;

    const char* getViewProviderName() const override
    {
        return "MeasureGui::ViewProviderMeasureLength";
    }

    static bool isValidSelection(const App::MeasureSelection& selection);
    void parseSelection(const App::MeasureSelection& selection) override;

    std::vector<std::string> getInputProps() override
    {
        return {"Elements"};
    }
    App::Property* getResultProp() override
    {
        return &this->Length;
    }

    // Return a placement for the viewprovider, just use the first element for now
    Base::Placement getPlacement() const override;

    // Return the object we are measuring
    std::vector<App::DocumentObject*> getSubject() const override;


private:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Measure


#endif  // MEASURE_MEASURELENGTH_H
