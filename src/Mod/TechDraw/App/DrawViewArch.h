#ifndef DrawViewArch_h_
#define DrawViewArch_h_

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Base/BoundBox.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewSymbol.h"


namespace TechDraw
{

class TechDrawExport DrawViewArch : public TechDraw::DrawViewSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewArch);

public:
    /// Constructor
    DrawViewArch();
    ~DrawViewArch() override = default;

    App::PropertyLink         Source;
    App::PropertyBool         AllOn;
    App::PropertyEnumeration  RenderMode; // "Wireframe", "Solid"
    App::PropertyBool         FillSpaces;
    App::PropertyBool         ShowHidden;
    App::PropertyBool         ShowFill;
    App::PropertyFloat        LineWidth;
    App::PropertyFloat        FontSize;
    App::PropertyFloat        CutLineWidth;
    App::PropertyBool         JoinArch;
    App::PropertyFloat        LineSpacing;


    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderArch";
    }

    short mustExecute() const override;


protected:
/*    virtual void onChanged(const App::Property* prop) override;*/
    Base::BoundBox3d bbox;
    std::string getSVGHead();
    std::string getSVGTail();

private:
    static const char* RenderModeEnums[];
};

} //namespace TechDraw


#endif
