#ifndef DrawViewClip_h_
#define DrawViewClip_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"

// ?? this (and DrawViewCollection) could use App::GroupExtension instead??

namespace TechDraw
{

class TechDrawExport DrawViewClip: public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewClip);

public:
    /// Constructor
    DrawViewClip();
    ~DrawViewClip() override = default;

    App::PropertyLength Width;
    App::PropertyLength Height;
    App::PropertyBool ShowFrame;
    App::PropertyLinkList Views;

    void addView(App::DocumentObject* docObj);
    void removeView(App::DocumentObject* docObj);
    short mustExecute() const override;

    std::vector<App::DocumentObject*> getViews() const;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderViewClip";
    }
    //return PyObject as DrawViewClipPy
    PyObject *getPyObject() override;

    std::vector<std::string> getChildViewNames();
    bool isViewInClip(App::DocumentObject* view);
    QRectF getRect() const override { return { 0, 0, Width.getValue(), Height.getValue() };  }


protected:
    void onChanged(const App::Property* prop) override;
};

using DrawViewClipPython = App::FeaturePythonT<DrawViewClip>;

} //namespace TechDraw


#endif
