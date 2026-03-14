#ifndef DRAWINGGUI_QGRAPHICSITEMLEADERLINE_H
#define DRAWINGGUI_QGRAPHICSITEMLEADERLINE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPointF>
#include <QStyleOptionGraphicsItem>

#include <Base/Vector3D.h>

#include "QGIView.h"
#include "QGIUserTypes.h"


namespace TechDraw
{
class DrawLeaderLine;
class DrawView;
}// namespace TechDraw

namespace TechDrawGui
{
class QGIPrimPath;
class QGIArrow;
class QGEPath;


//*******************************************************************

class TechDrawGuiExport QGILeaderLine: public QGIView
{
    Q_OBJECT

public:
    enum {Type = UserType::QGILeaderLine};

    explicit QGILeaderLine();
    ~QGILeaderLine() override = default;

    int type() const override
    {
        return Type;
    }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    QRectF boundingRect() const override;

    void drawBorder() override;
    void updateView(bool update = false) override;

    // leaders are not draggable
    void dragFinished() override { };

    virtual TechDraw::DrawLeaderLine* getLeaderFeature();

    void startPathEdit();
    void setArrows(std::vector<QPointF> pathPoints);

    void abandonEdit();
    void closeEdit();

    double getLineWidth();
    double getEdgeFuzz() const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void setPrettyNormal();
    void setPrettyPre();
    void setPrettySel();

    void setLeaderFeature(TechDraw::DrawLeaderLine* feat);

    bool useOldCoords() const;
    Base::Vector3d getAttachPoint();


public Q_SLOTS:
    void onLineEditFinished(QPointF tipDisplace,
                            std::vector<QPointF> points);//QGEPath is finished editing points

Q_SIGNALS:
    void editComplete();//tell caller that edit session is finished

protected:
    void draw() override;
    QPainterPath makeLeaderPath(std::vector<QPointF> qPoints);
    std::vector<QPointF> getWayPointsFromFeature();
    QPointF getAttachFromFeature();

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    void saveState();
    void restoreState();

    QColor prefNormalColor();
    void setNormalColorAll();

private:
    std::vector<QPointF> m_pathPoints;
    QGIPrimPath* m_line;//actual leader line
    QColor m_lineColor;
    Qt::PenStyle m_lineStyle;
    QGIArrow* m_arrow1;
    QGIArrow* m_arrow2;

    QGEPath* m_editPath;//line editor
    QColor m_editPathColor;

    bool m_hasHover;

    double m_saveX;
    double m_saveY;
    std::vector<Base::Vector3d> m_savePoints;

    bool m_blockDraw;//prevent redraws while updating.
};

}// namespace TechDrawGui

#endif// DRAWINGGUI_QGRAPHICSITEMLEADERLINE_H
