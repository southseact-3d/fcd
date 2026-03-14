#ifndef PARTGUI_SOBREPPOINTSET_H
#define PARTGUI_SOBREPPOINTSET_H

#include <Inventor/nodes/SoPointSet.h>
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

class PartGuiExport SoBrepPointSet: public SoPointSet
{
    using inherited = SoPointSet;

    SO_NODE_HEADER(SoBrepPointSet);

public:
    static void initClass();
    SoBrepPointSet();

    void setViewProvider(ViewProviderPartExt* vp)
    {
        viewProvider = vp;
    }

protected:
    ~SoBrepPointSet() override = default;
    void GLRender(SoGLRenderAction* action) override;
    void GLRenderBelowPath(SoGLRenderAction* action) override;
    void doAction(SoAction* action) override;

    void getBoundingBox(SoGetBoundingBoxAction* action) override;

private:
    using SelContext = Gui::SoFCSelectionContext;
    using SelContextPtr = Gui::SoFCSelectionContextPtr;
    void renderHighlight(SoGLRenderAction* action, SelContextPtr);
    void renderSelection(SoGLRenderAction* action, SelContextPtr, bool push = true);

private:
    SelContextPtr selContext;
    SelContextPtr selContext2;
    Gui::SoFCSelectionCounter selCounter;
    uint32_t packedColor {0};

    // backreference to viewprovider that owns this node
    ViewProviderPartExt* viewProvider = nullptr;
};

}  // namespace PartGui


#endif  // PARTGUI_SOBREPPOINTSET_H
