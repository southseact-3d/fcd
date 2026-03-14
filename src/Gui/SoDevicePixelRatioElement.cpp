#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/misc/SoState.h>


#include "SoDevicePixelRatioElement.h"

SO_ELEMENT_SOURCE(SoDevicePixelRatioElement);

void SoDevicePixelRatioElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoDevicePixelRatioElement, SoFloatElement);
    // Ensure the element is enabled for GLRenderAction
    SO_ENABLE(SoGLRenderAction, SoDevicePixelRatioElement);
}

void SoDevicePixelRatioElement::init(SoState* state)
{
    SoFloatElement::init(state);
    data = 1.0f;  // Default to a device pixel ratio of 1.0
}

void SoDevicePixelRatioElement::set(SoState* state, float dpr)
{
    SoFloatElement::set(classStackIndex, state, dpr);
}

float SoDevicePixelRatioElement::get(SoState* state)
{
    return SoFloatElement::get(classStackIndex, state);
}

SoDevicePixelRatioElement::~SoDevicePixelRatioElement()
{}
