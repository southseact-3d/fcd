#ifndef PART_FEATUREPARTCIRCLE_H
#define PART_FEATUREPARTCIRCLE_H

#include <App/PropertyUnits.h>

#include <Mod/Part/PartGlobal.h>

#include "PrimitiveFeature.h"

namespace Part
{
class PartExport Circle: public Part::Primitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Circle);

public:
    Circle();
    ~Circle() override;

    App::PropertyLength Radius;
    App::PropertyAngle Angle1;
    App::PropertyAngle Angle2;

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void onChanged(const App::Property*) override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderCircleParametric";
    }

protected:
    void Restore(Base::XMLReader& reader) override;
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* TypeName,
        const char* PropName
    ) override;

private:
    static App::PropertyQuantityConstraint::Constraints angleRange;
    //@}
};

}  // namespace Part

#endif  // PART_FEATUREPARTCIRCLE_H
