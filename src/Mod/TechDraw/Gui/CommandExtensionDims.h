#ifndef TECHDRAWGUI_CommandExtensionDims_H
#define TECHDRAWGUI_CommandExtensionDims_H

#include <Mod/TechDraw/TechDrawGlobal.h>

namespace TechDraw
{
    class ReferenceEntry;
    class DrawViewDimension;
}

namespace TechDrawGui {
    TechDraw::DrawViewDimension* makeArcLengthDimension(const TechDraw::ReferenceEntry& ref);

    std::vector<TechDraw::DrawViewDimension*> makeObliqueChainDimension(std::vector<TechDraw::ReferenceEntry> refs);
    std::vector<TechDraw::DrawViewDimension*> makeObliqueCoordDimension(std::vector<TechDraw::ReferenceEntry> refs);
}

#endif  // TECHDRAWGUI_CommandExtensionDims_H
