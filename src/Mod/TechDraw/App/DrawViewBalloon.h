#ifndef TechDraw_DrawViewBalloon_h_
#define TechDraw_DrawViewBalloon_h_

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


class TopoDS_Shape;

namespace Measure
{
class Measurement;
}
namespace TechDraw
{

class DrawViewPart;

class TechDrawExport DrawViewBalloon: public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewBalloon);

public:
    /// Constructor
    DrawViewBalloon();
    ~DrawViewBalloon() override;

    App::PropertyLink SourceView;
    App::PropertyString Text;
    App::PropertyEnumeration EndType;
    App::PropertyEnumeration BubbleShape;
    App::PropertyFloatConstraint ShapeScale;
    App::PropertyFloatConstraint EndTypeScale;
    App::PropertyDistance OriginX;
    App::PropertyDistance OriginY;
    App::PropertyFloat TextWrapLen;
    App::PropertyDistance KinkLength;

    short mustExecute() const override;

    DrawView* getParentView() const;
    QPointF origin;//WF never used??
    QPointF getOrigin();
    void setOrigin(QPointF p);

    //virtual PyObject *getPyObject(void);

    App::DocumentObjectExecReturn* execute() override;

    const char* getViewProviderName() const override { return "TechDrawGui::ViewProviderBalloon"; }

    static const char* balloonTypeEnums[];

    void handleXYLock() override;

    void setOrigin(Base::Vector3d newOrigin);

    Base::Vector3d getOriginOffset() const;

    App::PropertyLink *getOwnerProperty() override { return &SourceView; }

protected:
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyType(Base::XMLReader& reader, const char* TypeName,
                                   App::Property* prop) override;
    void handleChangedPropertyName(Base::XMLReader& reader, const char* TypeName,
                                   const char* PropName) override;

private:
    static App::PropertyFloatConstraint::Constraints SymbolScaleRange;
};

}//namespace TechDraw
#endif
