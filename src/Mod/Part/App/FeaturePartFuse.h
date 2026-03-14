#ifndef PART_FEATUREPARTFUSE_H
#define PART_FEATUREPARTFUSE_H

#include <Mod/Part/PartGlobal.h>

#include "FeaturePartBoolean.h"


namespace Part
{

class Fuse: public Boolean
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Fuse);

public:
    Fuse();

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
protected:
    BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const override;
    const char* opCode() const override;
    //@}
};

class PartExport MultiFuse: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::MultiFuse);

public:
    MultiFuse();

    App::PropertyLinkList Shapes;
    PropertyShapeHistory History;
    App::PropertyBool Refine;

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderMultiFuse";
    }
};

}  // namespace Part

#endif  // PART_FEATUREPARTFUSE_H
