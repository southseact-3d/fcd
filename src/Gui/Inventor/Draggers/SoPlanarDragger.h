#ifndef GUI_PLANAR_DRAGGER_H
#define GUI_PLANAR_DRAGGER_H

#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFDouble.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/nodes/SoBaseColor.h>

namespace Gui
{
/*! @brief Planar Translation Dragger.
 *
 * used for translating on a plane. Set the
 * translationIncrement to desired step. Use
 * 'translationIncrementXCount' or
 * 'translationIncrementYCount' multiplied with
 * 'translationIncrement' for a full double
 * precision vector scalar.
 *
 * @author qewer33
 */
class SoPlanarDragger: public SoDragger
{
    SO_KIT_HEADER(SoLinearDragger);
    SO_KIT_CATALOG_ENTRY_HEADER(planarTranslatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(planarTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(planarTranslatorActive);

public:
    static void initClass();
    SoPlanarDragger();
    SoSFVec3f translation;  //!< set from outside and used from outside for single precision.
    SoSFDouble translationIncrement;       //!< set from outside and used for rounding.
    SoSFInt32 translationIncrementXCount;  //!< number of steps. used from outside.
    SoSFInt32 translationIncrementYCount;  //!< number of steps. used from outside.
    SoSFFloat autoScaleResult;             //!< set from parent dragger.

protected:
    ~SoPlanarDragger() override;
    SbBool setUpConnections(SbBool onoff, SbBool doitalways = FALSE) override;

    static void startCB(void*, SoDragger* d);
    static void motionCB(void*, SoDragger* d);
    static void finishCB(void*, SoDragger* d);
    static void fieldSensorCB(void* f, SoSensor*);
    static void valueChangedCB(void*, SoDragger* d);

    void dragStart();
    void drag();
    void dragFinish();

    SoFieldSensor fieldSensor;
    SbPlaneProjector projector;

private:
    void buildFirstInstance();
    SbVec3f roundTranslation(const SbVec3f& vecIn, float incrementIn);
    SoGroup* buildGeometry();
    using inherited = SoDragger;
};

}  // namespace Gui

#endif /* GUI_PLANAR_DRAGGER_H */
