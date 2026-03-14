#ifndef TechDraw_LandmarkDimension_h_
#define TechDraw_LandmarkDimension_h_

# include <App/DocumentObject.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewDimension.h"

class TopoDS_Shape;
class gp_Ax2;

namespace Measure {
class Measurement;
}
namespace TechDraw
{
class DrawViewPart;

class TechDrawExport LandmarkDimension : public TechDraw::DrawViewDimension
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::LandmarkDimension);

public:
    /// Constructor
    LandmarkDimension();
    ~LandmarkDimension() override;

    App::PropertyStringList  ReferenceTags;     //tags of 2d vertices in DVP

    App::DocumentObjectExecReturn *execute() override;
    short mustExecute() const override;
    void unsetupObject() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderDimension"; }

    DrawViewPart* getViewPart() const override;
    RefType getRefType() const override;

    gp_Ax2 getProjAxis() const;

protected:
    void onChanged(const App::Property* prop) override;
    void onDocumentRestored() override;

    Base::Vector3d projectPoint(const Base::Vector3d& pt, DrawViewPart* dvp) const;

private:
};

} //namespace TechDraw
#endif
