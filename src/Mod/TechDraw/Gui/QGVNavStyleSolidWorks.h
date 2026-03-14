#ifndef TECHDRAW_SOLIDWORKSNAVSTYLE_H
#define TECHDRAW_SOLIDWORKSNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleSolidWorks : public QGVNavStyle
{
public:
    explicit QGVNavStyleSolidWorks(QGVPage* qgvp);
    ~QGVNavStyleSolidWorks() override;

    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_SOLIDWORKSNAVSTYLE_H
