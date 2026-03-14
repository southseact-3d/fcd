#ifndef PART_FEATUREPARTBOOLEAN_H
#define PART_FEATUREPARTBOOLEAN_H

#include <App/PropertyLinks.h>

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"

class FCBRepAlgoAPI_BooleanOperation;

namespace Part
{

class PartExport Boolean: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Boolean);

public:
    Boolean();

    App::PropertyLink Base;
    App::PropertyLink Tool;
    PropertyShapeHistory History;
    App::PropertyBool Refine;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderBoolean";
    }

protected:
    virtual BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const
        = 0;
    virtual const char* opCode() const = 0;
};

}  // namespace Part

#endif  // PART_FEATUREPARTBOOLEAN_H
