#ifndef TECHDRAW_TINKERNAVSTYLE_H
#define TECHDRAW_TINKERNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleTinkerCAD : public QGVNavStyle
{
//    TYPESYSTEM_HEADER();

public:
    explicit QGVNavStyleTinkerCAD(QGVPage* qgvp);
    ~QGVNavStyleTinkerCAD() override;

    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_TINKERNAVSTYLE_H
