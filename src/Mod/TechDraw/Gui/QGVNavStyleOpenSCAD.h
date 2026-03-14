#ifndef TECHDRAW_OPENSCADNAVSTYLE_H
#define TECHDRAW_OPENSCADNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleOpenSCAD : public QGVNavStyle
{
public:
    explicit QGVNavStyleOpenSCAD(QGVPage* qgvp);
    ~QGVNavStyleOpenSCAD() override;

    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_OPENSCADNAVSTYLE_H
