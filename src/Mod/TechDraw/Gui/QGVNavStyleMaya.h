#ifndef TECHDRAW_MAYANAVSTYLE_H
#define TECHDRAW_MAYANAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleMaya : public QGVNavStyle
{
public:
    explicit QGVNavStyleMaya(QGVPage* qgvp);
    ~QGVNavStyleMaya() override;

    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_MAYANAVSTYLE_H
