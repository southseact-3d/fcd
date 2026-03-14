#ifndef SKETCHERGUI_SOZOOMTRANSLATION_H
#define SKETCHERGUI_SOZOOMTRANSLATION_H

#include <Inventor/nodes/SoTranslation.h>

#include <Mod/Sketcher/SketcherGlobal.h>


namespace SketcherGui
{

class SketcherGuiExport SoZoomTranslation: public SoTranslation
{
    using inherited = SoTranslation;

    SO_NODE_HEADER(SoZoomTranslation);

public:
    static void initClass();
    SoZoomTranslation();
    SoSFVec3f abPos;
    float getScaleFactor() const
    {
        return scaleFactor;
    }

protected:
    ~SoZoomTranslation() override
    {}
    void doAction(SoAction* action) override;
    void getPrimitiveCount(SoGetPrimitiveCountAction* action) override;
    void getMatrix(SoGetMatrixAction* action) override;
    void GLRender(SoGLRenderAction* action) override;
    void getBoundingBox(SoGetBoundingBoxAction* action) override;
    void callback(SoCallbackAction* action) override;
    void pick(SoPickAction* action) override;
    float calculateScaleFactor(SoAction* action) const;

    mutable float scaleFactor;
};

}  // namespace SketcherGui
#endif  // SKETCHERGUI_SOZOOMTRANSLATION_H
