#ifndef GUI_INVENTOR_SODRAWINGGRID_H
#define GUI_INVENTOR_SODRAWINGGRID_H

#include <Inventor/nodes/SoShape.h>
#include <FCGlobal.h>

namespace Gui
{
namespace Inventor
{

class GuiExport SoDrawingGrid: public SoShape
{
    using inherited = SoShape;

    SO_NODE_HEADER(SoDrawingGrid);

public:
    static void initClass();
    SoDrawingGrid();

public:
    void GLRender(SoGLRenderAction* action) override;
    void GLRenderBelowPath(SoGLRenderAction* action) override;
    void GLRenderInPath(SoGLRenderAction* action) override;
    void GLRenderOffPath(SoGLRenderAction* action) override;
    void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center) override;
    void generatePrimitives(SoAction* action) override;

private:
    void renderGrid(SoGLRenderAction* action);
    // Force using the reference count mechanism.
    ~SoDrawingGrid() override = default;
};

}  // namespace Inventor

}  // namespace Gui

#endif  // GUI_INVENTOR_SODRAWINGGRID_H
