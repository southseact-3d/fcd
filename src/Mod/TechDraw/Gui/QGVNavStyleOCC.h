#ifndef TECHDRAW_OCCNAVSTYLE_H
#define TECHDRAW_OCCNAVSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGVNavStyle.h"

namespace TechDrawGui {

class QGVPage;

class TechDrawGuiExport QGVNavStyleOCC : public QGVNavStyle
{
public:
    explicit QGVNavStyleOCC(QGVPage* qgvp);
    ~QGVNavStyleOCC() override;

    void handleKeyReleaseEvent(QKeyEvent *event) override;
    void handleMousePressEvent(QMouseEvent *event) override;
    void handleMouseMoveEvent(QMouseEvent *event) override;
    void handleMouseReleaseEvent(QMouseEvent *event) override;

    //context menu (RMB) prevents pan mode 2 (RMB or CNTL+RMB)
    bool allowContextMenu(QContextMenuEvent *event) override;

protected:
private:

};

}
#endif // TECHDRAW_OCCNAVSTYLE_H
