#ifndef GUI_SO3DANNOTATION_H
#define GUI_SO3DANNOTATION_H

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/elements/SoSubElement.h>
#include <FCGlobal.h>
#include <vector>

namespace Gui
{

class GuiExport SoDelayedAnnotationsElement: public SoElement
{
    using inherited = SoElement;

    SO_ELEMENT_HEADER(SoDelayedAnnotationsElement);

protected:
    ~SoDelayedAnnotationsElement() override = default;

    SoDelayedAnnotationsElement& operator=(const SoDelayedAnnotationsElement& other) = default;
    SoDelayedAnnotationsElement& operator=(SoDelayedAnnotationsElement&& other) noexcept = default;

    // internal structure to hold path with it's rendering
    // priority (lower renders first)
    struct PriorityPath
    {
        SoPath* path;
        int priority;

        PriorityPath(SoPath* p, int pr = 0)
            : path(p)
            , priority(pr)
        {}
    };

public:
    SoDelayedAnnotationsElement(const SoDelayedAnnotationsElement& other) = delete;
    SoDelayedAnnotationsElement(SoDelayedAnnotationsElement&& other) noexcept = delete;

    void init(SoState* state) override;

    static void initClass();

    static void addDelayedPath(SoState* state, SoPath* path, int priority = 0);

    static SoPathList getDelayedPaths(SoState* state);

    static void processDelayedPathsWithPriority(SoState* state, SoGLRenderAction* action);

    static bool isProcessingDelayedPaths;

    SbBool matches([[maybe_unused]] const SoElement* element) const override
    {
        return FALSE;
    }

    SoElement* copyMatchInfo() const override
    {
        return nullptr;
    }

private:
    std::vector<PriorityPath> paths;
};

/*! @brief 3D Annotation Node - Annotation with depth buffer
 *
 * This class is just like SoAnnotation with the difference that it does not disable
 * the depth buffer instead it clears it and renders on top of everything with proper
 * depth control.
 *
 * It should be used with caution as it does clear the depth buffer for each annotation!
 */
class GuiExport So3DAnnotation: public SoSeparator
{
    using inherited = SoSeparator;

    SO_NODE_HEADER(So3DAnnotation);

public:
    static bool render;

    So3DAnnotation();

    So3DAnnotation(const So3DAnnotation& other) = delete;
    So3DAnnotation(So3DAnnotation&& other) noexcept = delete;
    So3DAnnotation& operator=(const So3DAnnotation& other) = delete;
    So3DAnnotation& operator=(So3DAnnotation&& other) noexcept = delete;

    static void initClass();

    void GLRender(SoGLRenderAction* action) override;
    void GLRenderBelowPath(SoGLRenderAction* action) override;
    void GLRenderInPath(SoGLRenderAction* action) override;
    void GLRenderOffPath(SoGLRenderAction* action) override;

protected:
    ~So3DAnnotation() override = default;
};

}  // namespace Gui

#endif  // GUI_SO3DANNOTATION_H
