#ifndef TECHDRAWGUI_QGIGHOSTHIGHLIGHT_H
#define TECHDRAWGUI_QGIGHOSTHIGHLIGHT_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsScene>
#include <QObject>
#include <QPointF>

#include "QGIHighlight.h"
#include "QGIUserTypes.h"


//a movable, selectable surrogate for detail highlights in QGIVPart

namespace TechDrawGui
{

class TechDrawGuiExport QGIGhostHighlight : public QObject, public QGIHighlight
{
    Q_OBJECT
public:
    explicit QGIGhostHighlight();
    ~QGIGhostHighlight() override;

    enum {Type = UserType::QGIGhostHighlight};
    int type() const override { return Type;}

    void setInteractive(bool state);
    void setRadius(double r);

Q_SIGNALS:
    void positionChange(QPointF p);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    bool m_dragging;

private:
};

}

#endif // TECHDRAWGUI_QGIGHOSTHIGHLIGHT_H
