#ifndef TECHDRAWGUI_QGIRICHANNO_H
#define TECHDRAWGUI_QGIRICHANNO_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QFont>
#include <QGraphicsItem>
#include <QPen>
#include <QStyleOptionGraphicsItem>

#include "QGIView.h"
#include "QGIUserTypes.h"


namespace TechDraw {
class DrawRichAnno;
class DrawLeaderLine;
}

namespace TechDrawGui
{
class QGIPrimPath;
class QGIArrow;
class QGEPath;
class QGMText;
class QGCustomText;
class QGCustomRect;


//*******************************************************************

class TechDrawGuiExport QGIRichAnno : public QGIView
{
    Q_OBJECT

public:
    enum {Type = UserType::QGIRichAnno};

    explicit QGIRichAnno();
    ~QGIRichAnno() override = default;

    int type() const override { return Type;}
    void paint( QPainter * painter,
                const QStyleOptionGraphicsItem * option,
                QWidget * widget = nullptr ) override;
    QRectF boundingRect() const override;

    void drawBorder() override;
    void updateView(bool update = false) override;

    void setTextItem();

    virtual TechDraw::DrawRichAnno* getFeature();
    QPen rectPen() const;

    void setExportingPdf(bool b) { m_isExportingPdf = b; }
    bool getExportingPdf() const { return m_isExportingPdf; }
    void setExportingSvg(bool b) { m_isExportingSvg = b; }
    bool getExportingSvg() const { return m_isExportingSvg; }

protected:
    void draw() override;
    void setLineSpacing(int lineSpacing);
    QFont prefFont(void);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    QString convertTextSizes(const QString& inHtml)  const;

    bool m_isExportingPdf;
    bool m_isExportingSvg;
    QGCustomText* m_text;
    bool m_hasHover;
    QGCustomRect* m_rect;

};

}

#endif // TECHDRAWGUI_QGIRICHANNO_H
