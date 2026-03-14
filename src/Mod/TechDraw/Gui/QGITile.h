#ifndef TECHDRAWGUI_QGITILE_H
#define TECHDRAWGUI_QGITILE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QFont>
#include <QGraphicsColorizeEffect>
#include <QPointF>

#include "QGIDecoration.h"
#include "QGIUserTypes.h"

namespace TechDraw {
class DrawTile;
class DrawTileWeld;
}

namespace TechDrawGui
{
class QGCustomSvg;
class QGCustomText;
class QGIWeldSymbol;

class TechDrawGuiExport QGITile : public QGIDecoration
{
public:
    explicit QGITile(TechDraw::DrawTileWeld*);
    ~QGITile() override = default;

    enum {Type = UserType::QGITile};
    int type() const override { return Type;}

    QRectF boundingRect() const override;

    void setTileTextLeft(const std::string& text);
    void setTileTextRight(const std::string& text);
    void setTileTextCenter(const std::string& text);
    void setFont(const QFont& font, double fSizePx);
    void setFont(const std::string& fName, double fSizePx);
    void setSymbolFile(const std::string& fileSpec);
    void setTilePosition(QPointF org, int row, int col);
    void setTileScale(double scale);
    void setTailRight(bool state) { m_tailRight = state; }
    void setAltWeld(bool state) { m_altWeld = state; }
    bool isTailRight() const;
    void draw() override;

    void setLocalAxes(Base::Vector3d xdir, Base::Vector3d ydir);
    QPointF mapPointToRotation(Base::Vector3d pointIn);
    QPointF calcTilePosition();

protected:
    QColor getTileColor() const;
    void setPrettyNormal();
    void setPrettyPre();
    void setPrettySel();

    double getSymbolWidth() const;
    double getSymbolHeight() const;
    double getSymbolFactor() const;
    QByteArray getSvgString(QString svgPath);

    QString prefTextFont() const;
    double prefFontSize() const;
    void makeSymbol();
    void makeText();

    bool getAltWeld() const;
    bool isReadable(QString filePath);
    std::string getStringFromFile(const std::string &inSpec);


private:
    QGCustomText*      m_qgTextL;
    QGCustomText*      m_qgTextR;
    QGCustomText*      m_qgTextC;
    QGCustomSvg*       m_qgSvg;
  //QGraphicsColorizeEffect* m_effect;
    QString            m_svgPath;
    QString            m_textL;
    QString            m_textR;
    QString            m_textC;
    QString            m_fontName;
    QFont              m_font;
    QPointF            m_origin;
    double             m_wide;
    double             m_high;
    double             m_scale;
    int                m_row;
    int                m_col;
    bool               m_tailRight;
    bool               m_altWeld;
    TechDraw::DrawTileWeld* m_tileFeat;

    Base::Vector3d     m_leaderXDirection;
    Base::Vector3d     m_leaderYDirection;
};

}

#endif // TECHDRAWGUI_QGITILE_H
