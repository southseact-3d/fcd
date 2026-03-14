#ifndef TECHDRAW_BLENDERNAVSTYLE_H
#define TECHDRAW_BLENDERNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleBlender : public QGVNavStyle
{
public:
    explicit QGVNavStyleBlender(QGVPage* qgvp);
    ~QGVNavStyleBlender() override;

    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    //context menu (RMB) prevents pan mode 2 (LMB + RMB)
    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_BLENDERNAVSTYLE_H
