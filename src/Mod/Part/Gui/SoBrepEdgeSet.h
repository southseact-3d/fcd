#ifndef PARTGUI_SOBREPEDGESET_H
#define PARTGUI_SOBREPEDGESET_H

#include <Inventor/nodes/SoIndexedLineSet.h>
#include <memory>
#include <vector>
#include <Gui/Selection/SoFCSelectionContext.h>
#include <Mod/Part/PartGlobal.h>


class SoCoordinateElement;
class SoGLCoordinateElement;
class SoTextureCoordinateBundle;

namespace PartGui
{

class ViewProviderPartExt;

class PartGuiExport SoBrepEdgeSet: public SoIndexedLineSet
{
    using inherited = SoIndexedLineSet;

    SO_NODE_HEADER(SoBrepEdgeSet);

public:
    static void initClass();
    SoBrepEdgeSet();

    void setViewProvider(ViewProviderPartExt* vp)
    {
        viewProvider = vp;
    }

protected:
    ~SoBrepEdgeSet() override = default;
    void GLRender(SoGLRenderAction* action) override;
    void GLRenderBelowPath(SoGLRenderAction* action) override;
    void doAction(SoAction* action) override;
    SoDetail* createLineSegmentDetail(
        SoRayPickAction* action,
        const SoPrimitiveVertex* v1,
        const SoPrimitiveVertex* v2,
        SoPickedPoint* pp
    ) override;

    void getBoundingBox(SoGetBoundingBoxAction* action) override;

private:
    struct SelContext;
    using SelContextPtr = std::shared_ptr<SelContext>;

    void renderShape(
        const SoGLCoordinateElement* const vertexlist,
        const int32_t* vertexindices,
        int num_vertexindices
    );
    void renderHighlight(SoGLRenderAction* action, SelContextPtr);
    void renderSelection(SoGLRenderAction* action, SelContextPtr, bool push = true);
    bool validIndexes(const SoCoordinateElement*, const std::vector<int32_t>&) const;


private:
    SelContextPtr selContext;
    SelContextPtr selContext2;
    Gui::SoFCSelectionCounter selCounter;
    uint32_t packedColor {0};

    // backreference to viewprovider that owns this node
    ViewProviderPartExt* viewProvider = nullptr;
};

}  // namespace PartGui


#endif  // PARTGUI_SOBREPEDGESET_H
