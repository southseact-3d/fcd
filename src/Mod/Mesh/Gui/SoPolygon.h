#ifndef MESHGUI_SOPOLYGON_H
#define MESHGUI_SOPOLYGON_H

#include <Inventor/elements/SoReplacedElement.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoShape.h>
#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif


namespace MeshGui
{

// NOLINTBEGIN
class MeshGuiExport SoPolygon: public SoShape
{
    using inherited = SoShape;

    SO_NODE_HEADER(SoPolygon);

public:
    static void initClass();
    SoPolygon();

    SoSFInt32 startIndex;
    SoSFInt32 numVertices;
    SoSFBool highlight;
    SoSFBool render;

protected:
    ~SoPolygon() override = default;
    void GLRender(SoGLRenderAction* action) override;
    void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center) override;
    void rayPick(SoRayPickAction* action) override;
    void generatePrimitives(SoAction* action) override;

private:
    void drawPolygon(const SbVec3f*, int32_t) const;
};
// NOLINTEND

}  // namespace MeshGui


#endif  // MESHGUI_SOPOLYGON_H
