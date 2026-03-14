#ifndef DRAWINGGUI_QGRAPHICSITEMWELDSYMBOL_H
#define DRAWINGGUI_QGRAPHICSITEMWELDSYMBOL_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QFont>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPointF>
#include <QStyleOptionGraphicsItem>

#include "QGIUserTypes.h"
#include "QGIView.h"


namespace App {
class Document;
}

namespace TechDraw {
class DrawWeldSymbol;
class DrawLeaderLine;
class DrawTileWeld;
class DrawView;
}

namespace TechDrawGui
{
class QGIPrimPath;
class QGITile;
class QGIVertex;
class QGCustomText;
class QGILeaderLine;

//*******************************************************************

class TechDrawGuiExport QGIWeldSymbol : public QGIView
{
    Q_OBJECT

public:
    enum {Type = UserType::QGIWeldSymbol};

    explicit QGIWeldSymbol();
    ~QGIWeldSymbol() override = default;

    int type() const override { return Type;}
    void paint( QPainter * painter,
                        const QStyleOptionGraphicsItem * option,
                        QWidget * widget = nullptr ) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    double getEdgeFuzz() const;

    void drawBorder() override;
    void updateView(bool update = false) override;

    virtual TechDraw::DrawWeldSymbol* getFeature();
    virtual TechDraw::DrawLeaderLine *getLeader();

    QPointF getTileOrigin();
    QPointF getKinkPoint();
    QPointF getTailPoint();

    void setPrettyNormal();
    void setPrettySel();
    void setPrettyPre();

    void getTileFeats();
    void makeLines();
    double getLastSegAngle();
    std::pair<Base::Vector3d, Base::Vector3d> getLocalAxes();

protected:
    QVariant itemChange( GraphicsItemChange change,
                                 const QVariant &value ) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void draw() override;
    void drawTile(TechDraw::DrawTileWeld* tileFeat);
    void drawAllAround();
    void drawTailText();
    void drawFieldFlag();

protected:
    void removeQGITiles();
    std::vector<QGITile*> getQGITiles() const;

    QColor prefNormalColor();
    double prefArrowSize() const;
    double prefFontSize() const;

    virtual QRectF customBoundingRect() const;

private:
    TechDraw::DrawTileWeld*   m_arrowFeat;
    TechDraw::DrawTileWeld*   m_otherFeat;
    std::string               m_arrowName;
    std::string               m_otherName;

    QGCustomText* m_tailText;
    QGIPrimPath* m_fieldFlag;
    QGIVertex* m_allAround;

    QFont m_font;

    bool m_blockDraw;    //prevent redraws while updating.


};

}

#endif // DRAWINGGUI_QGRAPHICSITEMWELDSYMBOL_H
