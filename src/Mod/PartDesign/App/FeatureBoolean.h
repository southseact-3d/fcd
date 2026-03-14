#ifndef PARTDESIGN_FeatureBoolean_H
#define PARTDESIGN_FeatureBoolean_H

#include <App/GeoFeatureGroupExtension.h>
#include <App/PropertyStandard.h>
#include "FeatureRefine.h"


namespace PartDesign
{

/**
 * Abstract superclass of all features that are created by transformation of another feature
 * Transformations are translation, rotation and mirroring
 */
class PartDesignExport Boolean: public PartDesign::FeatureRefine, public App::GeoFeatureGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartDesign::Boolean);

public:
    Boolean();

    /// The type of the boolean operation
    App::PropertyEnumeration Type;

    /** @name methods override feature */
    //@{
    /// Recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    void updatePreviewShape() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderBoolean";
    }
    void onChanged(const App::Property* prop) override;
    //@}

protected:
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* TypeName,
        const char* PropName
    ) override;


private:
    static const char* TypeEnums[];
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureBoolean_H
