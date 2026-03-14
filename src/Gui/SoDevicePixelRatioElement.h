#ifndef SO_DEVICE_PIXEL_RATIO_ELEMENT_H
#define SO_DEVICE_PIXEL_RATIO_ELEMENT_H

#include <Inventor/elements/SoFloatElement.h>

class SoState;

class SoDevicePixelRatioElement: public SoFloatElement
{
    SO_ELEMENT_HEADER(SoDevicePixelRatioElement);

public:
    // Initializes the class
    static void initClass();

    // Initializes the element
    virtual void init(SoState* state) override;

    // Sets the device pixel ratio
    static void set(SoState* state, float dpr);

    // Retrieves the device pixel ratio
    static float get(SoState* state);

protected:
    virtual ~SoDevicePixelRatioElement();
};

#endif  // SO_DEVICE_PIXEL_RATIO_ELEMENT_H
