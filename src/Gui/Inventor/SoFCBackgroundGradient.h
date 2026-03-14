#ifndef GUI_SOFCBACKGROUNDGRADIENT_H
#define GUI_SOFCBACKGROUNDGRADIENT_H

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSubNode.h>
#include <FCGlobal.h>


class SbColor;
class SoGLRenderAction;

namespace Gui
{

class GuiExport SoFCBackgroundGradient: public SoNode
{
    using inherited = SoNode;

    SO_NODE_HEADER(Gui::SoFCBackgroundGradient);

public:
    enum Gradient
    {
        LINEAR = 0,
        RADIAL = 1
    };
    static void initClass();
    static void finish();
    SoFCBackgroundGradient();

    void GLRender(SoGLRenderAction* action) override;
    void setGradient(Gradient grad);
    Gradient getGradient() const;
    void setColorGradient(const SbColor& fromColor, const SbColor& toColor);
    void setColorGradient(const SbColor& fromColor, const SbColor& toColor, const SbColor& midColor);

private:
    Gradient gradient;

protected:
    ~SoFCBackgroundGradient() override;

    SbColor fCol, tCol, mCol;
};

}  // namespace Gui


#endif  // GUI_SOFCBACKGROUNDGRADIENT_H
