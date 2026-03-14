#ifndef MEASURE_MEASUREDIAMETER_H
#define MEASURE_MEASUREDIAMETER_H

#include <Mod/Measure/MeasureGlobal.h>

#include <App/Application.h>
#include <App/GeoFeature.h>
#include <App/PropertyLinks.h>
#include <App/PropertyUnits.h>
#include <Base/Placement.h>

#include <Mod/Part/App/MeasureInfo.h>

#include "MeasureBase.h"

namespace Measure
{

class MeasureExport MeasureDiameter: public Measure::MeasureBaseExtendable<Part::MeasureRadiusInfo>
{
    PROPERTY_HEADER_WITH_OVERRIDE(Measure::MeasureDiameter);

public:
    MeasureDiameter();
    ~MeasureDiameter() override;

    App::PropertyLinkSub Element;
    App::PropertyDistance Diameter;

    App::DocumentObjectExecReturn* execute() override;
    const char* getViewProviderName() const override
    {
        return "MeasureGui::ViewProviderMeasureDiameter";
    }

    static bool isValidSelection(const App::MeasureSelection& selection);
    static bool isPrioritizedSelection(const App::MeasureSelection& selection);
    void parseSelection(const App::MeasureSelection& selection) override;

    std::vector<std::string> getInputProps() override
    {
        return {"Element"};
    }
    App::Property* getResultProp() override
    {
        return &this->Diameter;
    }

    Base::Placement getPlacement() const override;
    Base::Vector3d getPointOnCurve() const;

    std::vector<App::DocumentObject*> getSubject() const override;

private:
    void onChanged(const App::Property* prop) override;
    Part::MeasureRadiusInfoPtr getMeasureInfoFirst() const;
};

}  // namespace Measure

#endif  // MEASURE_MEASUREDIAMETER_H
