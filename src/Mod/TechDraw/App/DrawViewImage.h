#ifndef DrawViewImage_h_
#define DrawViewImage_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Base/BoundBox.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"

namespace TechDraw
{

class TechDrawExport DrawViewImage : public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewImage);

public:
    /// Constructor
    DrawViewImage();
    ~DrawViewImage() override = default;

    App::PropertyFile         ImageFile;
    App::PropertyFileIncluded ImageIncluded;
    App::PropertyFloat        Width;
    App::PropertyFloat        Height;
    App::PropertyLink         Owner;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderImage";
    }

    App::PropertyLink *getOwnerProperty() override { return &Owner; }

    QRectF getRect() const override;
    void setupObject() override;

protected:
    void replaceImageIncluded(std::string newImageFile);

    void onChanged(const App::Property* prop) override;
    Base::BoundBox3d bbox;
};

using DrawViewImagePython = App::FeaturePythonT<DrawViewImage>;


} //namespace TechDraw


#endif
