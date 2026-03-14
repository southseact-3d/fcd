#ifndef GUI_VIEWPROVIDER_FEATURE_H
#define GUI_VIEWPROVIDER_FEATURE_H

#include "ViewProviderDocumentObject.h"


class SbVec2s;

namespace Gui
{

class View3DInventorViewer;

class GuiExport ViewProviderFeature: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderFeature);

public:
    /// constructor.
    ViewProviderFeature();

    /// destructor.
    ~ViewProviderFeature() override;

    App::PropertyColorList ColourList;

    /**
     * Attaches the document object to this view provider.
     */
    void attach(App::DocumentObject* pcObj) override;
};

}  // namespace Gui

#endif  // GUI_VIEWPROVIDER_FEATURE_H
