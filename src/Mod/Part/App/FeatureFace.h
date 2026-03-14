#ifndef PART_FACE_H
#define PART_FACE_H

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Face: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Face);

public:
    Face();

    App::PropertyLinkList Sources;
    App::PropertyString FaceMakerClass;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderFace";
    }
    void setupObject() override;
    //@}
};

}  // namespace Part


#endif  // PART_FACE_H
