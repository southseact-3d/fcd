#ifndef TECHDRAW_FEATUREVIEWCOLLECTION_h_
#define TECHDRAW_FEATUREVIEWCOLLECTION_h_

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


namespace TechDraw
{

/** Base class for collection of view objects
 */
class TechDrawExport DrawViewCollection : public DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawViewCollection);

public:
    App::PropertyLinkList Views;
public:
    /// Constructor
    DrawViewCollection();
    ~DrawViewCollection() override;
    short mustExecute() const override;

    int addView(App::DocumentObject* obj);
    int removeView(App::DocumentObject* obj);
    std::vector<App::DocumentObject*> getViews() const;
    void rebuildViewList();
    bool isUnsetting() { return nowUnsetting; }

    int countChildren();
    void lockChildren();

    void onDocumentRestored() override;
    App::DocumentObjectExecReturn *execute() override;
    void unsetupObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderViewCollection";
    }
    QRectF getRect() const override;

protected:
    void onChanged(const App::Property* prop) override;
    bool nowUnsetting;
};

} //namespace TechDraw

#endif // TECHDRAW_FEATUREVIEWCOLLECTION_h_
