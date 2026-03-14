#ifndef PARTGUI_VIEWPROVIDERPOINTPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERPOINTPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderPointParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPointParametric);

public:
    /// constructor
    ViewProviderPointParametric();
    /// destructor
    ~ViewProviderPointParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPOINTPARAMETRIC_H
