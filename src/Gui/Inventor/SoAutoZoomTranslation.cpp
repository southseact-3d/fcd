#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/nodes/SoCamera.h>


#include "SoAutoZoomTranslation.h"


using namespace Gui;

// ------------------------------------------------------

SO_NODE_SOURCE(SoAutoZoomTranslation)

void SoAutoZoomTranslation::initClass()
{
    SO_NODE_INIT_CLASS(SoAutoZoomTranslation, SoTransformation, "AutoZoom");

    // Enable elements for SoGetMatrixAction (#0002268)
    // SoCamera::initClass() enables the SoViewVolumeElement for
    // * SoGLRenderAction
    // * SoGetBoundingBoxAction
    // * SoRayPickAction
    // * SoCallbackAction
    // * SoGetPrimitiveCountAction
    // The element SoViewportRegionElement is enabled by the
    // above listed actions.
    // Additionally, SoViewVolumeElement is enabled for
    // * SoAudioRenderAction
    // * SoHandleEventAction
    // And SoViewportRegionElement is enabled for
    // * SoHandleEventAction
    // * SoGetMatrixAction
    SO_ENABLE(SoGetMatrixAction, SoViewVolumeElement);
}

float SoAutoZoomTranslation::getScaleFactor(SoAction* action) const
{
    float scale = scaleFactor.getValue();
    if (!scale) {
        return 1.0;
    }
    // Dividing by 5 seems to work well
    SbViewVolume vv = SoViewVolumeElement::get(action->getState());
    float aspectRatio = SoViewportRegionElement::get(action->getState()).getViewportAspectRatio();
    scale *= vv.getWorldToScreenScale(SbVec3f(0.f, 0.f, 0.f), 0.1f) / (5 * aspectRatio);
    return scale;
}

SoAutoZoomTranslation::SoAutoZoomTranslation()
{
    SO_NODE_CONSTRUCTOR(SoAutoZoomTranslation);
    SO_NODE_ADD_FIELD(scaleFactor, (1.0f));
    // SO_NODE_ADD_FIELD(abPos, (SbVec3f(0.f,0.f,0.f)));
}

void SoAutoZoomTranslation::GLRender(SoGLRenderAction* action)
{
    SoAutoZoomTranslation::doAction(static_cast<SoAction*>(action));
    inherited::GLRender(action);
}

// Doc in superclass.
void SoAutoZoomTranslation::doAction(SoAction* action)
{
    float sf = this->getScaleFactor(action);
    auto state = action->getState();
    SbRotation r, so;
    SbVec3f s, t;
    SbMatrix matrix = SoModelMatrixElement::get(action->getState());  // clazy:exclude=rule-of-two-soft
    matrix.getTransform(t, r, s, so);
    matrix.multVecMatrix(SbVec3f(0, 0, 0), t);
    // reset current model scale factor
    matrix.setTransform(t, r, SbVec3f(sf, sf, sf));
    SoModelMatrixElement::set(state, this, matrix);
}

// set the auto scale factor.
// void SoAutoZoomTranslation::setAutoScale(void)
//{
//    float sf = this->getScaleFactor();
//    //this->enableNotify	(	false );
//    scaleFactor.setValue(SbVec3f(sf,sf,sf));
//    //this->enableNotify	(	true );
//    //scaleFactor.setDirty (true);
//
//}

void SoAutoZoomTranslation::getMatrix(SoGetMatrixAction* action)
{
    float sf = this->getScaleFactor(action);

    SbMatrix& m = action->getMatrix();

    SbRotation r, so;
    SbVec3f s, t;
    m.getTransform(t, r, s, so);
    m.multVecMatrix(SbVec3f(0, 0, 0), t);
    m.setTransform(t, r, SbVec3f(sf, sf, sf));

    action->getInverse() = m.inverse();
}

void SoAutoZoomTranslation::callback(SoCallbackAction* action)
{
    SoAutoZoomTranslation::doAction(static_cast<SoAction*>(action));
}

void SoAutoZoomTranslation::getBoundingBox(SoGetBoundingBoxAction* action)
{
    SoAutoZoomTranslation::doAction(static_cast<SoAction*>(action));
}

void SoAutoZoomTranslation::pick(SoPickAction* action)
{
    SoAutoZoomTranslation::doAction(static_cast<SoAction*>(action));
}

// Doc in superclass.
void SoAutoZoomTranslation::getPrimitiveCount(SoGetPrimitiveCountAction* action)
{
    SoAutoZoomTranslation::doAction(static_cast<SoAction*>(action));
}
