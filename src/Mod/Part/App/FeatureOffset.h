#ifndef PART_FEATUREOFFSET_H
#define PART_FEATUREOFFSET_H

#include <App/PropertyStandard.h>

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Offset: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Offset);

public:
    Offset();
    ~Offset() override;

    App::PropertyLink Source;
    App::PropertyFloat Value;
    App::PropertyEnumeration Mode;
    App::PropertyEnumeration Join;
    App::PropertyBool Intersection;
    App::PropertyBool SelfIntersection;
    App::PropertyBool Fill;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderOffset";
    }
    //@}

private:
    static const char* ModeEnums[];
    static const char* JoinEnums[];
};

class PartExport Offset2D: public Offset
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Offset2D);

public:
    Offset2D();
    ~Offset2D() override;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderOffset2D";
    }
    //@}
};

}  // namespace Part
#endif  // PART_FEATUREOFFSET_H
