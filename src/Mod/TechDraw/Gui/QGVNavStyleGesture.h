#ifndef TECHDRAW_GESTURENAVSTYLE_H
#define TECHDRAW_GESTURENAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleGesture : public QGVNavStyle
{
public:
    explicit QGVNavStyleGesture(QGVPage* qgvp);
    ~QGVNavStyleGesture() override;

    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_GESTURENAVSTYLE_H
