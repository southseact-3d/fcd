#ifndef GUI_SOFCSELECTION_H
#define GUI_SOFCSELECTION_H

#include <FCConfig.h>

#ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
#else
# ifdef FC_OS_WIN32
#  include <windows.h>
# endif
# include <GL/gl.h>
#endif

#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/nodes/SoGroup.h>

#include "SoFCSelectionContext.h"


class SoFullPath;
class SoPickedPoint;


namespace Gui
{


/** Selection node
 *  This node does the complete highlighting and selection together with the viewer
 *  \author Jürgen Riegel
 */
class GuiExport SoFCSelection: public SoGroup
{
    using inherited = SoGroup;

    SO_NODE_HEADER(Gui::SoFCSelection);

public:
    static void initClass();
    static void finish();
    SoFCSelection();

    /// Load highlight settings from the configuration
    void applySettings();

    enum PreselectionModes
    {
        AUTO,
        ON,
        OFF
    };

    enum SelectionModes
    {
        SEL_ON,
        SEL_OFF
    };

    enum Selected
    {
        NOTSELECTED,
        SELECTED
    };

    enum Styles
    {
        EMISSIVE,
        EMISSIVE_DIFFUSE,
        BOX
    };

    SbBool isHighlighted() const
    {
        return highlighted;
    }

    SoSFColor colorHighlight;
    SoSFColor colorSelection;
    SoSFEnum style;
    SoSFEnum selected;
    SoSFEnum preselectionMode;
    SoSFEnum selectionMode;

    SoSFString documentName;
    SoSFString objectName;
    SoSFString subElementName;
    SoSFBool useNewSelection;

    void doAction(SoAction* action) override;
    void GLRender(SoGLRenderAction* action) override;

    void handleEvent(SoHandleEventAction* action) override;
    void GLRenderBelowPath(SoGLRenderAction* action) override;
    void GLRenderInPath(SoGLRenderAction* action) override;
    static void turnOffCurrentHighlight(SoGLRenderAction* action);

protected:
    ~SoFCSelection() override;

    using SelContext = SoFCSelectionContext;
    using SelContextPtr = std::shared_ptr<SelContext>;
    SelContextPtr selContext;
    SelContextPtr selContext2;

    virtual void redrawHighlighted(SoAction* act, SbBool flag);

    SbBool readInstance(SoInput* in, unsigned short flags) override;

private:
    static int getPriority(const SoPickedPoint*);
    static void turnoffcurrent(SoAction* action);
    bool setOverride(SoGLRenderAction* action, SelContextPtr);
    SbBool isHighlighted(SoAction* action);
    SbBool preRender(SoGLRenderAction* act, GLint& oldDepthFunc);
    const SoPickedPoint* getPickedPoint(SoHandleEventAction*) const;

    static SoFullPath* currenthighlight;

    SbBool highlighted;
    SoColorPacker colorpacker;

    SbBool bShift;
    SbBool bCtrl;
};


}  // namespace Gui

#endif  // !GUI_SOFCSELECTION_H
