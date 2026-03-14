#ifndef TECHDRAWGUI_QGRAPHICSITEMVIEWDATUMLABEL_H
#define TECHDRAWGUI_QGRAPHICSITEMVIEWDATUMLABEL_H

#include <QGraphicsObject>

#include "Enums.h"
#include "QGCustomText.h"
#include "QGIUserTypes.h"
#include "QGIViewDimension.h"


namespace TechDrawGui {

class QGIDatumLabel : public QGraphicsObject
{
Q_OBJECT

public:
    QGIDatumLabel();
    ~QGIDatumLabel() override = default;

    enum {Type = UserType::QGIDatumLabel};
    int type() const override { return Type;}

    QRectF boundingRect() const override;
    QRectF tightBoundingRect() const;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint( QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget = nullptr ) override;
    void setLabelCenter();
    void setPosFromCenter(const double &xCenter, const double &yCenter);
    double X() const { return x() + getPosToCenterVec().x; }
    double Y() const { return y() + getPosToCenterVec().y; }
    Base::Vector2d getPosToCenterVec() const;

    void setFont(QFont font);
    QFont getFont() const { return m_dimText->font(); }
    void setDimString(QString text, qreal maxWidth=-1);
    void setToleranceString();
    void setPrettySel();
    void setPrettyPre();
    void setPrettyNormal();
    void setColor(QColor color);
    void setSelectability(bool val);
    void setFrameColor(QColor color);

    QGCustomText* getDimText() { return m_dimText; }
    void setDimText(QGCustomText* newText) { m_dimText = newText; }
    QGCustomText* getTolTextOver() { return m_tolTextOver; }
    void setTolTextOver(QGCustomText* newTol) { m_tolTextOver = newTol; }
    QGCustomText* getTolTextUnder() { return m_tolTextUnder; }
    void setTolTextUnder(QGCustomText* newTol) { m_tolTextOver = newTol; }

    double getTolAdjust();

    bool isFramed() const { return m_frame->parentItem(); }  // If empty pointer, then no frame
    void setFramed(bool framed);

    double getLineWidth() const { return m_frame->pen().widthF(); }
    void setLineWidth(double lineWidth);
    void setQDim(QGIViewDimension* qDim) { parent = qDim;}

Q_SIGNALS:
    void setPretty(int state);
    void dragging(bool);
    void hover(bool state);
    void selected(bool state);
    void dragFinished();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void updateFrameRect();
    void updateChildren();
    void boundingRectChanged();

    int getPrecision();

    void snapPosition(QPointF& position);

    bool getVerticalSep() const { return verticalSep; }
    void setVerticalSep(bool sep) { verticalSep = sep; }
    std::vector<int> getSeps() const { return seps; }
    void setSeps(std::vector<int> newSeps) { seps = newSeps; }

private:
    bool verticalSep;
    std::vector<int> seps;

    QGIViewDimension* parent;

    QGCustomText* m_dimText;
    QGCustomText* m_tolTextOver;
    QGCustomText* m_tolTextUnder;
    QGCustomText* m_unitText;
    QGraphicsItemGroup* m_textItems;
    QGraphicsRectItem* m_frame;
    QColor m_colNormal;
    bool m_ctrl;

    DragState m_dragState;

private:
};

}  // namespace TechDrawGui

#endif
