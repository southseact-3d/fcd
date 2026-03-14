#ifndef DRAWINGGUI_QGRAPHICSITEMCMARK_H
#define DRAWINGGUI_QGRAPHICSITEMCMARK_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIUserTypes.h"
#include "QGIVertex.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGICMark : public QGIVertex
{
public:
    explicit QGICMark(int index);
    ~QGICMark() override {}

    enum {Type = UserType::QGICMark};
    int type() const override { return Type;}
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void draw(void);
    float getSize() { return m_size; }
    void setSize(float s);
    float getThick() { return m_pen.widthF(); }
    void setThick(float t);
    void setPrettyNormal() override;

    double getMarkFuzz(void) const;

protected:
    QColor getCMarkColor();

private:
    float m_size;
};

}

#endif // DRAWINGGUI_QGRAPHICSITEMCMARK_H
