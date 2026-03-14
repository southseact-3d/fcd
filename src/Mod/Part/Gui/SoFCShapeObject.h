#ifndef PARTGUI_SOFCSHAPEOBJECT_H
#define PARTGUI_SOFCSHAPEOBJECT_H

#include "SoBrepEdgeSet.h"
#include "SoBrepFaceSet.h"
#include "SoBrepPointSet.h"

#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFUInt32.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>

#include <Mod/Part/PartGlobal.h>

namespace PartGui
{

class PartGuiExport SoFCShape: public SoSeparator
{
    using inherited = SoSeparator;
    SO_NODE_HEADER(SoFCShape);

public:
    SoFCShape();
    static void initClass();

    SoCoordinate3* coords;
    SoNormal* norm;
    SoBrepFaceSet* faceset;
    SoBrepEdgeSet* lineset;
    SoBrepPointSet* nodeset;
};

class PartGuiExport SoFCControlPoints: public SoShape
{
    using inherited = SoShape;

    SO_NODE_HEADER(SoFCControlPoints);

public:
    static void initClass();
    SoFCControlPoints();

    SoSFUInt32 numPolesU;
    SoSFUInt32 numPolesV;
    SoSFUInt32 numKnotsU;
    SoSFUInt32 numKnotsV;
    SoSFColor lineColor;

protected:
    ~SoFCControlPoints() override = default;
    ;
    void GLRender(SoGLRenderAction* action) override;
    void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center) override;
    void generatePrimitives(SoAction* action) override;

private:
    void drawControlPoints(const SbVec3f*, int32_t) const;
};

}  // namespace PartGui


#endif  // PARTGUI_SOFCSHAPEOBJECT_H
