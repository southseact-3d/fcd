#ifndef GUI_VIEWPROVIDER_ViewProviderGeoFeatureGroup_H
#define GUI_VIEWPROVIDER_ViewProviderGeoFeatureGroup_H


#include "ViewProviderDocumentObject.h"
#include "ViewProviderGeoFeatureGroupExtension.h"
#include "ViewProviderFeaturePython.h"

namespace Gui
{

class GuiExport ViewProviderGeoFeatureGroup: public ViewProviderDocumentObject,
                                             public ViewProviderGeoFeatureGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Gui::ViewProviderGeoFeatureGroup);

public:
    /// constructor.
    ViewProviderGeoFeatureGroup();
    /// destructor.
    ~ViewProviderGeoFeatureGroup() override;
};

using ViewProviderGeoFeatureGroupPython = ViewProviderFeaturePythonT<ViewProviderGeoFeatureGroup>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDER_ViewProviderGeoFeatureGroup_H
