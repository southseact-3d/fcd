#ifndef Sandbox_WORKBENCH_H
#define Sandbox_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Inventor/nodes/SoShape.h>
#include <QPointer>
#include <QImage>

namespace SandboxGui {

class Workbench : public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
    Workbench();
    virtual ~Workbench();

protected:
    Gui::MenuItem* setupMenuBar() const;
    Gui::ToolBarItem* setupToolBars() const;
    Gui::ToolBarItem* setupCommandBars() const;
    Gui::DockWindowItems* setupDockWindows() const;
};

class SoWidgetShape : public SoShape {
    using inherited = SoShape;

    SO_NODE_HEADER(SoWidgetShape);

public:
    static void initClass();
    SoWidgetShape();
    void setWidget(QWidget* w);

protected:
    virtual void GLRender(SoGLRenderAction *action);
    virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center);
    virtual void generatePrimitives(SoAction *action);
    void getQuad(SoState * state, SbVec3f & v0, SbVec3f & v1, SbVec3f & v2, SbVec3f & v3);

private:
    virtual ~SoWidgetShape(){};
    QPointer<QWidget> w;
    QImage image;
};

} // namespace SandboxGui


#endif // Sandbox_WORKBENCH_H 
