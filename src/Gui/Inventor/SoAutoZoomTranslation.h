#ifndef GUI_SOAUTOZOOMTRANSLATION_H
#define GUI_SOAUTOZOOMTRANSLATION_H

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoTransformation.h>
#include <FCGlobal.h>


namespace Gui
{

class GuiExport SoAutoZoomTranslation: public SoTransformation
{
    using inherited = SoTransformation;

    SO_NODE_HEADER(SoAutoZoomTranslation);

public:
    static void initClass();
    SoAutoZoomTranslation();

    SoSFFloat scaleFactor;

protected:
    ~SoAutoZoomTranslation() override = default;
    void doAction(SoAction* action) override;
    void getPrimitiveCount(SoGetPrimitiveCountAction* action) override;
    void getMatrix(SoGetMatrixAction* action) override;
    void GLRender(SoGLRenderAction* action) override;
    void getBoundingBox(SoGetBoundingBoxAction* action) override;
    void callback(SoCallbackAction* action) override;
    void pick(SoPickAction* action) override;
    float getScaleFactor(SoAction*) const;

private:
};

}  // namespace Gui
#endif  // GUI_SOAUTOZOOMTRANSLATION_H
