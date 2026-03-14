#ifndef DrawViewMulti_h_
#define DrawViewMulti_h_

#include <TopoDS_Compound.hxx>

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewPart.h"


class gp_Pln;
class TopoDS_Face;

namespace TechDraw
{
//class Face;
}

namespace TechDraw
{


/** Base class of all View Features in the drawing module
 */
class TechDrawExport DrawViewMulti : public DrawViewPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewMulti);

public:
    /// Constructor
    DrawViewMulti();
    ~DrawViewMulti() override;

    App::PropertyLinkList    Sources;

    short mustExecute() const override;
    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    void onChanged(const App::Property* prop) override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderViewPart";
    }

protected:
    TopoDS_Compound m_compound;

//    void getParameters(void);
};

using DrawViewMultiPython = App::FeaturePythonT<DrawViewMulti>;

} //namespace TechDraw

#endif
