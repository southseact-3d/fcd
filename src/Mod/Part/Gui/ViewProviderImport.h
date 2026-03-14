#ifndef PARTGUI_VIEVPROVIDERIMPORT_H
#define PARTGUI_VIEVPROVIDERIMPORT_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderImport: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderImport);

public:
    /// constructor
    ViewProviderImport();
    /// destructor
    ~ViewProviderImport() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEVPROVIDERIMPORT_H
