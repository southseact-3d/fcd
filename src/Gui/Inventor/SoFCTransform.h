#ifndef GUI_INVENTOR_SOFCTRANSFORM_H
#define GUI_INVENTOR_SOFCTRANSFORM_H

#include <Inventor/nodes/SoTransform.h>
#include <FCGlobal.h>

namespace Gui
{

/**
 * @class SoFCTransform
 * @brief A temporary workaround for coin3d/coin#534.
 *
 * This class is a workaround for a missing feature to reduce the OpenGL stack size.
 * The issue was reported here: https://github.com/coin3d/coin/issues/534
 * And was merged here: https://github.com/coin3d/coin/pull/535
 *
 * Once this feature is available in all supported versions of Coin3D, this class should
 * be removed and all instances should revert to using SoTransform.
 */
class GuiExport SoFCTransform: public SoTransform
{
    using inherited = SoTransform;

    SO_NODE_HEADER(SoFCTransform);

public:
    static void initClass();
    SoFCTransform();

protected:
    void GLRender(SoGLRenderAction* action) override;
    void callback(SoCallbackAction* action) override;
    void pick(SoPickAction* action) override;
    void getPrimitiveCount(SoGetPrimitiveCountAction* action) override;
    void getBoundingBox(SoGetBoundingBoxAction* action) override;
    void doAction(SoAction* action) override;
};

}  // namespace Gui

#endif  // GUI_INVENTOR_SOFCTRANSFORM_H
