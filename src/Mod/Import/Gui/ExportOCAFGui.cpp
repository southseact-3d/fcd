#include "ExportOCAFGui.h"
#include <Gui/Application.h>
#include <Mod/Part/Gui/ViewProvider.h>

using namespace ImportGui;

ExportOCAFGui::ExportOCAFGui(Handle(TDocStd_Document) hDoc, bool explicitPlacement)
    : ExportOCAF(hDoc, explicitPlacement)
{}

void ExportOCAFGui::findColors(Part::Feature* part, std::vector<Base::Color>& colors) const
{
    if (auto vp = Gui::Application::Instance->getViewProvider(part)) {
        if (auto vppe = freecad_cast<PartGui::ViewProviderPartExt*>(vp)) {
            colors = vppe->ShapeAppearance.getDiffuseColors();
            auto transp = vppe->ShapeAppearance.getTransparency();
            for (auto& it : colors) {
                it.setTransparency(transp);
            }
        }
    }
}
