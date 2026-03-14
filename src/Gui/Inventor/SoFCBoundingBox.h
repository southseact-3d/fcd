#ifndef COIN_SOFCBOUNDINGBOX_H
#define COIN_SOFCBOUNDINGBOX_H

#include <Inventor/actions/SoAction.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <FCGlobal.h>


namespace Gui
{

/**
 * A subclass of SoShape used to create an axis aligned wire frame box based
 * on the minBounds and maxBounds fields.  The class also has a field which
 * can be toggled on or off for displaying text coordinate labels at the
 * vertices of the box.
 * @note Original source are taken from
 * http://www.wheatchex.com/projects/openinventor/bbox/BoundingBox.h and
 * http://www.wheatchex.com/projects/openinventor/bbox/BoundingBox.cpp
 * @date November 12th, 2001
 * @author Josh Grant
 */
class GuiExport SoFCBoundingBox: public SoShape
{
    using inherited = SoShape;

    SO_NODE_HEADER(Gui::SoFCBoundingBox);

public:
    static void initClass();
    static void finish();

public:
    SoFCBoundingBox();
    SoSFVec3f minBounds;   /**< minimum box coordinates */
    SoSFVec3f maxBounds;   /**< maximum box coordinates */
    SoSFBool coordsOn;     /**< If true, the coordinates are displayed at each vertex */
    SoSFBool dimensionsOn; /**< If true, the dimensions are displayed in x,y and z direction */


protected:
    ~SoFCBoundingBox() override;
    void GLRender(SoGLRenderAction* action) override;
    void generatePrimitives(SoAction* action) override;
    void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center) override;

private:
    SoSeparator *root, *textSep, *dimSep;
    SoCoordinate3* bboxCoords;
    SoIndexedLineSet* bboxLines;
};

/**
 * This is a special group node which must be defined to ignore bounding box actions.
 * @author Werner Mayer
 */
class GuiExport SoSkipBoundingGroup: public SoGroup
{
    using inherited = SoGroup;

    SO_NODE_HEADER(Gui::SoSkipBoundingGroup);

public:
    static void initClass();
    static void finish();
    SoSkipBoundingGroup();

    enum Modes
    {
        INCLUDE_BBOX,
        EXCLUDE_BBOX
    };

    SoSFEnum mode;

    void getBoundingBox(SoGetBoundingBoxAction* action) override;

protected:
    ~SoSkipBoundingGroup() override;
};

}  // namespace Gui


#endif  // COIN_SOFCBOUNDINGBOX_H
