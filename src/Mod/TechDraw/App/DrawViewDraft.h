#ifndef DrawViewDraft_h_
#define DrawViewDraft_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyLinks.h>
#include <Base/BoundBox.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewSymbol.h"


namespace TechDraw
{

class TechDrawExport DrawViewDraft : public TechDraw::DrawViewSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewDraft);

public:
    /// Constructor
    DrawViewDraft();
    ~DrawViewDraft() override = default;

    App::PropertyLink         Source;
    App::PropertyFloat        LineWidth;
    App::PropertyFloat        FontSize;
    App::PropertyVector       Direction;
    App::PropertyColor        Color;
    App::PropertyString       LineStyle;
    App::PropertyFloat        LineSpacing;
    App::PropertyBool         OverrideStyle;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderDraft";
    }

    short mustExecute() const override;

protected:
/*    virtual void onChanged(const App::Property* prop) override;*/
    Base::BoundBox3d bbox;
    std::string getSVGHead();
    std::string getSVGTail();
};

using DrawViewDraftPython = App::FeaturePythonT<DrawViewDraft>;


} //namespace TechDraw


#endif
