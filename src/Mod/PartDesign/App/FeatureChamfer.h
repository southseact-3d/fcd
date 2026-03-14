#ifndef PARTDESIGN_FEATURECHAMFER_H
#define PARTDESIGN_FEATURECHAMFER_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include "FeatureDressUp.h"

namespace PartDesign
{

class PartDesignExport Chamfer: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Chamfer);

public:
    Chamfer();

    App::PropertyEnumeration ChamferType;
    App::PropertyQuantityConstraint Size;
    App::PropertyQuantityConstraint Size2;
    App::PropertyAngle Angle;
    App::PropertyBool FlipDirection;
    App::PropertyBool UseAllEdges;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderChamfer";
    }
    //@}

    void onChanged(const App::Property* /*prop*/) override;

    void updateProperties();

protected:
    void Restore(Base::XMLReader& reader) override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    static const App::PropertyQuantityConstraint::Constraints floatSize;
    static const App::PropertyAngle::Constraints floatAngle;

    bool requiresSizeSwapping(const Base::XMLReader& reader) const;
    void migrateFlippedProperties(const Base::XMLReader& reader);
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FEATURECHAMFER_H
