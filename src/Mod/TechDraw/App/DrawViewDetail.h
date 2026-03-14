#ifndef DrawViewDetail_h_
#define DrawViewDetail_h_

#include <gp_Ax2.hxx>
#include <TopoDS_Shape.hxx>

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewPart.h"


class gp_Pln;
class gp_Ax2;
class TopoDS_Face;

namespace TechDraw
{
class Face;
}

namespace TechDraw
{


class TechDrawExport DrawViewDetail : public DrawViewPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewDetail);

public:
    /// Constructor
    DrawViewDetail();
    ~DrawViewDetail() override;

    App::PropertyLink   BaseView;
    App::PropertyVector AnchorPoint;
    App::PropertyFloat   Radius;
    App::PropertyString Reference;

    App::PropertyBool   ShowMatting;
    App::PropertyBool   ShowHighlight;

    short mustExecute() const override;
    App::DocumentObjectExecReturn *execute() override;
    void onChanged(const App::Property* prop) override;
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderViewPart";
    }
    void unsetupObject() override;
    void handleChangedPropertyType(
        Base::XMLReader &reader, const char * TypeName, App::Property * prop) override;


    void detailExec(TopoDS_Shape& s,
                    DrawViewPart* baseView,
                    DrawViewSection* sectionAlias);
    void makeDetailShape(const TopoDS_Shape& shape,
                         DrawViewPart* dvp,
                         DrawViewSection* dvs);
    void postHlrTasks(void) override;
    void waitingForDetail(bool s) { m_waitingForDetail = s; }
    bool waitingForDetail(void) const { return m_waitingForDetail; }
    bool waitingForResult() const override;

    double getFudgeRadius(void);
    TopoDS_Shape projectEdgesOntoFace(TopoDS_Shape& edgeShape,
                                      TopoDS_Face& projFace,
                                      gp_Dir& projDir);

    std::vector<DrawViewDetail*> getDetailRefs() const override;
    TopoDS_Shape getDetailShape() const { return m_detailShape; }

    Base::Vector3d mapPoint3dToDetail(const Base::Vector3d& inPoint) const;

public Q_SLOTS:
    void onMakeDetailFinished(void);

protected:
    void getParameters(void);
    double m_fudge;
    bool debugDetail() const;

    TopoDS_Shape m_scaledShape;
    gp_Ax2 m_viewAxis;

    QMetaObject::Connection connectDetailWatcher;
    QFutureWatcher<void> m_detailWatcher;
    QFuture<void> m_detailFuture;
    bool m_waitingForDetail;

    DrawViewPart* m_saveDvp;
    DrawViewSection* m_saveDvs;

    TopoDS_Shape m_detailShape;
};

using DrawViewDetailPython = App::FeaturePythonT<DrawViewDetail>;

} //namespace TechDraw

#endif
