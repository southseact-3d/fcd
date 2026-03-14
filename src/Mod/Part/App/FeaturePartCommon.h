#ifndef PART_FEATUREPARTCOMMON_H
#define PART_FEATUREPARTCOMMON_H

#include <Mod/Part/PartGlobal.h>

#include "FeaturePartBoolean.h"


namespace Part
{

class Common: public Boolean
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Common);

public:
    Common();

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
protected:
    BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const override;
    const char* opCode() const override;
    //@}
};

enum CommonBehavior
{
    CommonOfAllShapes,
    CommonOfFirstAndRest,
};

class PartExport MultiCommon: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::MultiCommon);

public:
    MultiCommon();

    App::PropertyLinkList Shapes;
    PropertyShapeHistory History;
    App::PropertyBool Refine;
    App::PropertyEnumeration Behavior;

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}

    void Restore(Base::XMLReader& reader) override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderMultiCommon";
    }

private:
    static const char* BehaviorEnums[];
};

}  // namespace Part

#endif  // PART_FEATUREPARTCOMMON_H
