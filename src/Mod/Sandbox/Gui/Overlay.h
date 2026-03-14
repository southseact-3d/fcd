#ifndef SANDBOXGUI_OVERLAY_H
#define SANDBOXGUI_OVERLAY_H

#include <Gui/MouseSelection.h>
#include <QColor>
#include <QPoint>
#include <QList>

namespace SandboxGui {
class DrawingPlane : public Gui::BaseMouseSelection
{
public:
    DrawingPlane();
    virtual ~DrawingPlane();

protected:
    void initialize();
    void terminate();
    virtual int mouseButtonEvent( const SoMouseButtonEvent * const e, const QPoint& pos );
    virtual int locationEvent   ( const SoLocation2Event   * const e, const QPoint& pos );
    virtual int keyboardEvent   ( const SoKeyboardEvent    * const e );
    void draw ();

private:
    void drawLineTo(const QPoint &endPoint);

    bool scribbling;
    int myPenWidth;
    float myRadius;
    QColor myPenColor;
    QPoint lastPoint;
    QList<QPoint> selection;

    QOpenGLFramebufferObject* fbo;
};

} // SandboxGui

#endif // SANDBOXGUI_OVERLAY_H
