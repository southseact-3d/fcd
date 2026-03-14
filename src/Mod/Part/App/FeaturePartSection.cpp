#include <Mod/Part/App/FCBRepAlgoAPI_Section.h>
#include <Standard_Version.hxx>


#include "FeaturePartSection.h"
#include "TopoShapeOpCode.h"

using namespace Part;

PROPERTY_SOURCE(Part::Section, Part::Boolean)


Section::Section()
{
    ADD_PROPERTY_TYPE(Approximation, (false), "Section", App::Prop_None, "Approximate the output edges");
}

short Section::mustExecute() const
{
    if (Approximation.isTouched()) {
        return 1;
    }
    return 0;
}


const char* Section::opCode() const
{
    return Part::OpCodes::Section;
}

BRepAlgoAPI_BooleanOperation* Section::makeOperation(
    const TopoDS_Shape& base,
    const TopoDS_Shape& tool
) const
{
    // Let's call algorithm computing a section operation:

    bool approx = Approximation.getValue();
    std::unique_ptr<FCBRepAlgoAPI_Section> mkSection(new FCBRepAlgoAPI_Section());
    mkSection->Init1(base);
    mkSection->Init2(tool);
    mkSection->Approximation(approx);
    mkSection->setAutoFuzzy();
    mkSection->Build();
    if (!mkSection->IsDone()) {
        throw Base::RuntimeError("Section failed");
    }
    return mkSection.release();
}
