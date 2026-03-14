#ifndef TECHDRAW_CADNAVSTYLE_H
#define TECHDRAW_CADNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleCAD : public QGVNavStyle
{
public:
    explicit QGVNavStyleCAD(QGVPage* qgvp);
    ~QGVNavStyleCAD() override;

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
#endif // TECHDRAW_CADNAVSTYLE_H
