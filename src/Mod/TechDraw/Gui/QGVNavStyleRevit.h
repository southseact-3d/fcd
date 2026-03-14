#ifndef TECHDRAW_REVITNAVSTYLE_H
#define TECHDRAW_REVITNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleRevit : public QGVNavStyle
{
public:
    explicit QGVNavStyleRevit(QGVPage* qgvp);
    ~QGVNavStyleRevit() override;

    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_REVITNAVSTYLE_H
