#ifndef PARTGUI_VIEWPROVIDERPART_H
#define PARTGUI_VIEWPROVIDERPART_H

#include <utility>

#include <Gui/ViewProviderBuilder.h>
#include <Mod/Part/Gui/ViewProviderExt.h>
#include <Mod/Part/PartGlobal.h>

class SoSeparator;

namespace Gui
{
class GizmoContainer;
}

namespace Part
{
struct ShapeHistory;
}

namespace PartGui
{

class ViewProviderShapeBuilder: public Gui::ViewProviderBuilder
{
public:
    ViewProviderShapeBuilder() = default;
    ~ViewProviderShapeBuilder() override = default;
    void buildNodes(const App::Property*, std::vector<SoNode*>&) const override;
    void createShape(const App::Property*, SoSeparator*) const;
};

class PartGuiExport ViewProviderPart: public ViewProviderPartExt
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPart);

public:
    /// constructor
    ViewProviderPart();
    /// destructor
    ~ViewProviderPart() override;
    bool doubleClicked() override;

protected:
    void applyColor(
        const Part::ShapeHistory& hist,
        const std::vector<Base::Color>& colBase,
        std::vector<Base::Color>& colBool
    );
    void applyMaterial(
        const Part::ShapeHistory& hist,
        const std::vector<App::Material>& colBase,
        std::vector<App::Material>& colBool
    );
    void applyTransparency(float transparency, std::vector<Base::Color>& colors);
    void applyTransparency(float transparency, std::vector<App::Material>& colors);
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPART_H
