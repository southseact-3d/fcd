#ifndef TechDraw_DrawViewDimExtent_h_
#define TechDraw_DrawViewDimExtent_h_

#include <tuple>

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewDimension.h"


namespace TechDraw {

class TechDrawExport DrawViewDimExtent : public TechDraw::DrawViewDimension
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewDimExtent);

public:
    /// Constructor
    DrawViewDimExtent();
    ~DrawViewDimExtent() override = default;

    App::PropertyLinkSubList       Source;                       //DrawViewPart & SubElements(Edges)
                                                                 //Cosmetic End points are stored in DVD::References2d
    App::PropertyLinkSubList       Source3d;                     //Part::Feature & SubElements  TBI
    App::PropertyInteger           DirExtent;                    //Horizontal, Vertical, TBD
    App::PropertyStringList        CosmeticTags;                 //id of cosmetic end points.  obsolete!

    App::DocumentObjectExecReturn *execute() override;

    RefType getRefType() const override { return RefType::extent; }

    PyObject *getPyObject() override;

protected:
    virtual pointPair getPointsExtent(ReferenceVector references);
    bool checkReferences2D() const override;

private:
};

} //namespace TechDraw
#endif
