#ifndef TechDrawGUI_TechDrawHandler_H
#define TechDrawGUI_TechDrawHandler_H

#include <Gui/ToolHandler.h>

#include <Mod/TechDraw/TechDrawGlobal.h>

namespace TechDrawGui
{
class QGVPage;

class TechDrawGuiExport TechDrawHandler : public Gui::ToolHandler
{
public:

    TechDrawHandler();
    virtual ~TechDrawHandler();

    void activate(QGVPage* vPage);
    void deactivate() override;

    void quit() override;

    virtual void mouseMoveEvent(QMouseEvent* event) = 0;
    virtual void mousePressEvent(QMouseEvent* event) { Q_UNUSED(event) };
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void keyPressEvent(QKeyEvent* event) = 0;
    virtual void keyReleaseEvent(QKeyEvent* event);

    TechDraw::DrawPage* getPage();


protected:
    QWidget* getCursorWidget() override;
    void setWidgetCursor(QCursor cursor) override;

    QGVPage* viewPage;
};


}  // namespace TechDrawGui


#endif  // TechDrawGUI_TechDrawHandler_H
