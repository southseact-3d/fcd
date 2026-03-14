#ifndef PART_FEATUREMIRRORING_H
#define PART_FEATUREMIRRORING_H

#include <App/PropertyStandard.h>

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Mirroring: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Mirroring);

public:
    Mirroring();

    App::PropertyLink Source;
    App::PropertyPosition Base;
    App::PropertyDirection Normal;
    App::PropertyLinkSub MirrorPlane;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderMirror";
    }
    //@}

protected:
    void onChanged(const App::Property* prop) override;

    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
};

}  // namespace Part


#endif  // PART_FEATUREMIRRORING_H
