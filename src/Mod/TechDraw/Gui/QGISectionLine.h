#ifndef TECHDRAWGUI_QGISECTIONLINE_H
#define TECHDRAWGUI_QGISECTIONLINE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QFont>
#include <QPainterPath>
#include <QPointF>

#include <Base/Vector3D.h>
#include <Mod/TechDraw/App/DrawViewSection.h>

#include "QGCustomText.h"
#include "QGIDecoration.h"
#include "QGIUserTypes.h"


namespace TechDrawGui
{

class QGIArrow;
class QGCustomText;

class TechDrawGuiExport QGISectionLine : public QGIDecoration
{
public:
    explicit QGISectionLine();
    ~QGISectionLine() override = default;

    enum {Type = UserType::QGISectionLine};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;

    void setEnds(Base::Vector3d l1, Base::Vector3d l2);
    void setBounds(double x1, double y1, double x2, double y2);
    void setPath(QPainterPath& path);
    void setSymbol(char* sym);
    void setDirection(double xDir, double yDir);
    void setDirection(Base::Vector3d dir);
    void setArrowDirections(Base::Vector3d dir1, Base::Vector3d dir2);
    void setFont(QFont f, double fsize);
    void setSectionColor(QColor c);
    void setPathMode(bool mode) { m_pathMode = mode; }
    void setShowLine(bool state) { m_showLine = state; }
    bool pathMode() { return m_pathMode; }
    void setChangePoints(TechDraw::ChangePointVector changePoints);
    void clearChangePoints();
    void draw() override;

    void setLinePen(QPen isoPen);


protected:
    QColor getSectionColor();
    Qt::PenStyle getSectionStyle();
    void makeSectionLine();
    void makeExtensionLine();
    void makeArrows();
    void makeArrowsTrad();
    void makeArrowsISO();
    void makeSymbols();
    void makeSymbolsTrad();
    void makeSymbolsISO();
    void makeChangePointMarks();
    void setTools();
    void extensionEndsISO();
    void extensionEndsTrad();
    double getArrowRotation(Base::Vector3d arrowDir);
    QPointF getArrowPosition(Base::Vector3d arrowDir, QPointF refPoint);
    void clearChangePointMarks();

    static QPointF normalizeQPointF(QPointF inPoint);

private:
    const char*        m_symbol;
    QGraphicsPathItem* m_line;
    QGraphicsPathItem* m_extend;
    QGIArrow*          m_arrow1;
    QGIArrow*          m_arrow2;
    QGCustomText*      m_symbol1;
    QGCustomText*      m_symbol2;
    QPointF            m_start;         //start of section line
    QPointF            m_end;           //end of section line
    Base::Vector3d     m_arrowDir;
    std::string        m_symFontName;
    QFont              m_symFont;
    double             m_symSize;
    double             m_arrowSize;
    double             m_extLen;
    Base::Vector3d     m_l1;            //end of main section line
    Base::Vector3d     m_l2;            //end of main section line
    QPointF            m_beginExt1;     //start of extension line 1
    QPointF            m_endExt1;       //end of extension line 1
    QPointF            m_beginExt2;     //start of extension line 2
    QPointF            m_endExt2;       //end of extension line 1
    bool               m_pathMode;      //use external path for line
    int                m_arrowMode;     //0 = 1 direction for both arrows, 1 = direction for each arrow
    Base::Vector3d     m_arrowDir1;
    Base::Vector3d     m_arrowDir2;
    QPointF            m_arrowPos1;
    QPointF            m_arrowPos2;
    std::vector<QGraphicsPathItem*> m_changePointMarks;
    TechDraw::ChangePointVector m_changePointData;

    bool m_showLine{true};
};

}

#endif // TECHDRAWGUI_QGISECTIONLINE_H
