#ifndef PARTGUI_VIEWPROVIDERPLANEPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERPLANEPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderPlaneParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPlaneParametric);

public:
    /// constructor
    ViewProviderPlaneParametric();
    /// destructor
    ~ViewProviderPlaneParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

class PartGuiExport ViewProviderFace: public ViewProviderPlaneParametric
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderFace);

public:
    ViewProviderFace();
    ~ViewProviderFace() override;

    std::vector<App::DocumentObject*> claimChildren() const override;
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPLANEPARAMETRIC_H
