#include <Mod/Part/App/FCBRepAlgoAPI_Cut.h>


#include "FeaturePartCut.h"
#include "TopoShapeOpCode.h"

using namespace Part;

PROPERTY_SOURCE(Part::Cut, Part::Boolean)


Cut::Cut() = default;

const char* Cut::opCode() const
{
    return Part::OpCodes::Cut;
}

BRepAlgoAPI_BooleanOperation* Cut::makeOperation(const TopoDS_Shape& base, const TopoDS_Shape& tool) const
{
    // Let's call algorithm computing a cut operation:
    return new FCBRepAlgoAPI_Cut(base, tool);
}
