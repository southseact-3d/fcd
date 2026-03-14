#ifndef VIEWPROVIDERORIGINGROUP_H
#define VIEWPROVIDERORIGINGROUP_H

#include "ViewProviderOriginGroupExtension.h"

namespace Gui
{

class GuiExport ViewProviderOriginGroup: public ViewProviderDocumentObject,
                                         public ViewProviderOriginGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Gui::ViewProviderOriginGroup);

public:
    ViewProviderOriginGroup();
    ~ViewProviderOriginGroup() override;
};

}  // namespace Gui


#endif /* end of include guard: VIEWPROVIDERORIGINGROUP_H */
