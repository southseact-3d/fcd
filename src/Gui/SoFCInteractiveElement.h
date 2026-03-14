#ifndef GUI_SOFCINTERACTIVEELEMENT_H
#define GUI_SOFCINTERACTIVEELEMENT_H

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/elements/SoReplacedElement.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSubNode.h>

#include <FCGlobal.h>

class QOpenGLWidget;

namespace Gui
{
/**
 * @author Werner Mayer
 */
class GuiExport SoFCInteractiveElement: public SoReplacedElement
{
    using inherited = SoReplacedElement;

    SO_ELEMENT_HEADER(SoFCInteractiveElement);

public:
    static void initClass();

    void init(SoState* state) override;
    static void set(SoState* const state, SoNode* const node, SbBool mode);
    static SbBool get(SoState* const state);
    static const SoFCInteractiveElement* getInstance(SoState* state);

protected:
    ~SoFCInteractiveElement() override;
    virtual void setElt(SbBool mode);

private:
    SbBool interactiveMode;
};

class GuiExport SoGLWidgetElement: public SoElement
{
    using inherited = SoElement;

    SO_ELEMENT_HEADER(SoGLWidgetElement);

public:
    static void initClass();

    void init(SoState* state) override;
    void push(SoState* state) override;
    void pop(SoState* state, const SoElement* prevTopElement) override;

    SbBool matches(const SoElement* element) const override;
    SoElement* copyMatchInfo() const override;

    static void set(SoState* state, QOpenGLWidget* window);
    static void get(SoState* state, QOpenGLWidget*& window);

protected:
    ~SoGLWidgetElement() override;

protected:
    QOpenGLWidget* window;
};

class GuiExport SoGLRenderActionElement: public SoElement
{
    using inherited = SoElement;

    SO_ELEMENT_HEADER(SoGLRenderActionElement);

public:
    static void initClass();

    void init(SoState* state) override;
    void push(SoState* state) override;
    void pop(SoState* state, const SoElement* prevTopElement) override;

    SbBool matches(const SoElement* element) const override;
    SoElement* copyMatchInfo() const override;

    static void set(SoState* state, SoGLRenderAction* action);
    static void get(SoState* state, SoGLRenderAction*& action);

protected:
    ~SoGLRenderActionElement() override;

protected:
    SoGLRenderAction* glRenderAction;
};

class GuiExport SoGLWidgetNode: public SoNode
{
    using inherited = SoNode;

    SO_NODE_HEADER(SoGLWidgetNode);

public:
    static void initClass();
    SoGLWidgetNode();

    QOpenGLWidget* window {nullptr};

    void doAction(SoAction* action) override;
    void GLRender(SoGLRenderAction* action) override;

protected:
    ~SoGLWidgetNode() override;
};

class GuiExport SoGLVBOActivatedElement: public SoElement
{
    using inherited = SoElement;

    SO_ELEMENT_HEADER(SoGLVBOActivatedElement);

public:
    static void initClass();

    void init(SoState* state) override;
    void push(SoState* state) override;
    void pop(SoState* state, const SoElement* prevTopElement) override;

    SbBool matches(const SoElement* element) const override;
    SoElement* copyMatchInfo() const override;

    static void set(SoState* state, SbBool);
    static void get(SoState* state, SbBool& active);

protected:
    ~SoGLVBOActivatedElement() override;

protected:
    SbBool active;
};

}  // namespace Gui

#endif  // GUI_SOFCINTERACTIVEELEMENT_H
