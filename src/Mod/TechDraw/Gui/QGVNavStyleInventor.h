#ifndef TECHDRAW_INVENTORNAVSTYLE_H
#define TECHDRAW_INVENTORNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleInventor : public QGVNavStyle
{
public:
    explicit QGVNavStyleInventor(QGVPage* qgvp);
    ~QGVNavStyleInventor() override;

    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_INVENTORNAVSTYLE_H
