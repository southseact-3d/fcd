#ifndef DrawViewAnnotation_h_
#define DrawViewAnnotation_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyUnits.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


namespace TechDraw
{

class TechDrawExport DrawViewAnnotation : public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewAnnotation);

public:
    /// Constructor
    DrawViewAnnotation();
    ~DrawViewAnnotation() override = default;

    App::PropertyStringList   Text;
    App::PropertyFont         Font;
    App::PropertyColor        TextColor;
    App::PropertyLength       TextSize;
    App::PropertyInteger      LineSpace;
    App::PropertyEnumeration  TextStyle; // Plain, Bold, Italic, Bold-Italic
    App::PropertyFloat        MaxWidth;
    App::PropertyLink         Owner;

    QRectF getRect() const override;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderAnnotation";
    }

    short mustExecute() const override;

    bool checkFit() const override {return true;}
    App::PropertyLink *getOwnerProperty() override { return &Owner; }

protected:
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;

private:
    static const char* TextStyleEnums[];
};

using DrawViewAnnotationPython = App::FeaturePythonT<DrawViewAnnotation>;


} //namespace TechDraw


#endif
