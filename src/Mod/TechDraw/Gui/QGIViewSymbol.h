#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWSYMBOL_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWSYMBOL_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QByteArray>

#include "QGIView.h"
#include "QGIUserTypes.h"


namespace TechDraw {
class DrawViewSymbol;
}

namespace TechDrawGui
{
class QGCustomSvg;
class QGDisplayArea;

class TechDrawGuiExport QGIViewSymbol : public QGIView
{
public:
    QGIViewSymbol();
    ~QGIViewSymbol() override;

    enum {Type = UserType::QGIViewSymbol};
    int type() const override { return Type;}

    void updateView(bool update = false) override;
    void setViewSymbolFeature(TechDraw::DrawViewSymbol *obj);

    void draw() override;
    void rotateView() override;


protected:
    virtual void drawSvg();
    void symbolToSvg(QByteArray qba);
    double legacyScaler(TechDraw::DrawViewSymbol* feature) const;
    double symbolScaler(TechDraw::DrawViewSymbol* feature) const;

    QGDisplayArea* m_displayArea;
    QGCustomSvg *m_svgItem;
};

} // namespace
#endif // DRAWINGGUI_QGRAPHICSITEMVIEWSYMBOL_H
