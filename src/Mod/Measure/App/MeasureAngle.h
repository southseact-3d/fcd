#ifndef APP_MEASUREANGLE_H
#define APP_MEASUREANGLE_H

#include <Mod/Measure/MeasureGlobal.h>

#include <gp_Vec.hxx>

#include <App/PropertyGeo.h>
#include <App/PropertyLinks.h>
#include <App/PropertyUnits.h>
#include <Base/Vector3D.h>

#include <Mod/Part/App/MeasureInfo.h>

#include "MeasureBase.h"


namespace Measure
{


class MeasureExport MeasureAngle: public Measure::MeasureBaseExtendable<Part::MeasureAngleInfo>
{
    PROPERTY_HEADER_WITH_OVERRIDE(Measure::MeasureAngle);

public:
    /// Constructor
    MeasureAngle();
    ~MeasureAngle() override;

    App::PropertyLinkSub Element1;
    App::PropertyLinkSub Element2;
    App::PropertyAngle Angle;

    App::DocumentObjectExecReturn* execute() override;

    const char* getViewProviderName() const override
    {
        return "MeasureGui::ViewProviderMeasureAngle";
    }

    static bool isValidSelection(const App::MeasureSelection& selection);
    static bool isPrioritizedSelection(const App::MeasureSelection& selection);
    void parseSelection(const App::MeasureSelection& selection) override;

    std::vector<std::string> getInputProps() override
    {
        return {"Element1", "Element2"};
    }
    App::Property* getResultProp() override
    {
        return &this->Angle;
    }

    // Return the object we are measuring
    std::vector<App::DocumentObject*> getSubject() const override;


    static bool getVec(App::DocumentObject& ob, std::string& subName, Base::Vector3d& vecOut);
    Base::Vector3d getLoc(App::DocumentObject& ob, std::string& subName);

    // Orientation Vectors
    gp_Vec vector1();
    gp_Vec vector2();

    // Location Vectors
    gp_Vec location1();
    gp_Vec location2();

private:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Measure


#endif  // APP_MEASUREANGLE_H
