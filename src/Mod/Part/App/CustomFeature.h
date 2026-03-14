#ifndef PART_CUSTOMFEATURE_H
#define PART_CUSTOMFEATURE_H

#include <Mod/Part/PartGlobal.h>

#include <Mod/Part/App/PartFeature.h>

namespace Part
{

/** Base class of all custom feature classes which are almost used as base
 * for python feature classes.
 */
class PartExport CustomFeature: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::CustomFeature);

public:
    /// Constructor
    CustomFeature();
    ~CustomFeature() override;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderCustom";
    }
};

using CustomFeaturePython = App::FeaturePythonT<CustomFeature>;

}  // namespace Part


#endif  // PART_CUSTOMFEATURE_H
