#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>

#include "SoFCTransform.h"


using namespace Gui;

SO_NODE_SOURCE(SoFCTransform)

void SoFCTransform::initClass()
{
    SO_NODE_INIT_CLASS(SoFCTransform, SoTransform, "Transform");
}

SoFCTransform::SoFCTransform()
{
    SO_NODE_CONSTRUCTOR(SoFCTransform);
}

void SoFCTransform::GLRender(SoGLRenderAction* action)
{
    SoFCTransform::doAction(action);
}

void SoFCTransform::callback(SoCallbackAction* action)
{
    SoFCTransform::doAction(action);
}

void SoFCTransform::pick(SoPickAction* action)
{
    SoFCTransform::doAction(action);
}

void SoFCTransform::getPrimitiveCount(SoGetPrimitiveCountAction* action)
{
    SoFCTransform::doAction(action);
}

void SoFCTransform::getBoundingBox(SoGetBoundingBoxAction* action)
{
    SoFCTransform::doAction(action);
}

void SoFCTransform::doAction(SoAction* action)
{
    SbMatrix matrix;
    matrix.setTransform(
        this->translation.getValue(),
        this->rotation.getValue(),
        this->scaleFactor.getValue(),
        this->scaleOrientation.getValue(),
        this->center.getValue()
    );

    // This is different to SoTransform::doAction() where model matrix
    // is always set
    if (matrix != SbMatrix::identity()) {
        SoModelMatrixElement::mult(action->getState(), this, matrix);
    }
}
