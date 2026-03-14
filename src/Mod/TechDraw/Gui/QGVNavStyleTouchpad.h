#ifndef TECHDRAW_TOUCHPADNAVSTYLE_H
#define TECHDRAW_TOUCHPADNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleTouchpad : public QGVNavStyle
{
public:
    explicit QGVNavStyleTouchpad(QGVPage* qgvp);
    ~QGVNavStyleTouchpad() override;

    void handleKeyPressEvent(QKeyEvent *event) override;
    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;

protected:
    void setAnchor() override;

private:

};

}
#endif // TECHDRAW_TOUCHPADNAVSTYLE_H
