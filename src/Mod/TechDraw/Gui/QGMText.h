//QGMText.h
//a movable, editable text item

#ifndef TECHDRAWGUI_MOVABLETEXT_H
#define TECHDRAWGUI_MOVABLETEXT_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGCustomText.h"
#include "QGIUserTypes.h"


QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

//
class TechDrawGuiExport QGMText : public QGCustomText
{
Q_OBJECT

public:
    explicit QGMText(void);
    ~QGMText() override = default;

    enum {Type = UserType::QGMText};
    int type() const override { return Type;}
    void paint( QPainter * painter,
                const QStyleOptionGraphicsItem * option,
                QWidget * widget = nullptr ) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void showBox(bool b) { m_showBox = b; }
    virtual bool showBox() { return m_showBox; }

    void setPrettyNormal() override;
    void setPrettyPre() override;
    void setPrettySel() override;

Q_SIGNALS:
    void dragging();
    void hover(bool state);
    void selected(bool state);
    void dragFinished();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    bool m_showBox;
    std::string  m_prettyState;
    QPointF m_lastClick;

};

}

#endif // TECHDRAWGUI_MOVABLETEXT_H
