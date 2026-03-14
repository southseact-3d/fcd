#ifndef GUI_ARCENGINE_H
#define GUI_ARCENGINE_H

#include <FCGlobal.h>
#include <Inventor/fields/SoMFColor.h>
#include <Inventor/engines/SoEngineOutput.h>

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFMatrix.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodekits/SoSeparatorKit.h>

class SoText2;
class SoTranslation;
class SoCoordinate3;
class SoIndexedLineSet;

namespace Gui
{


// /*used for generating points for arc display*/
class GuiExport ArcEngine: public SoEngine
{
    SO_ENGINE_HEADER(ArcEngine);

public:
    ArcEngine();
    static void initClass();

    SoSFFloat radius;
    SoSFFloat angle;
    SoSFFloat deviation;

    SoEngineOutput points;
    SoEngineOutput pointCount;
    SoEngineOutput midpoint;

protected:
    void evaluate() override;

private:
    ~ArcEngine() override
    {}
    void defaultValues();  // some non error values if something goes wrong.
};

}  // namespace Gui

#endif  // GUI_ARCENGINE_H
