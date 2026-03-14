#ifndef DRAWINGGUI_QGRAPHICSITEMSVGTEMPLATE_H
#define DRAWINGGUI_QGRAPHICSITEMSVGTEMPLATE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

class QGraphicsScene;
class QGraphicsSvgItem;
class QSvgRenderer;
class QFile;
class QString;

#include "QGITemplate.h"
#include "QGIUserTypes.h"

namespace TechDraw
{
class DrawSVGTemplate;
}

namespace TechDrawGui
{
class QGSPage;

class TechDrawGuiExport QGISVGTemplate : public TechDrawGui::QGITemplate
{
    Q_OBJECT

public:
    explicit QGISVGTemplate(QGSPage* scene);
    ~QGISVGTemplate() override;

    enum {Type = UserType::QGISVGTemplate};
    int type() const override { return Type; }

    void draw() override;
    void drawPageRectangle();

    void updateView(bool update = false) override;

    TechDraw::DrawSVGTemplate* getSVGTemplate() const;
    std::vector<TemplateTextField*> getTextFields() override;

protected:
    void openFile(const QFile& file);
    void load(QByteArray svgCode);

    void createClickHandles();
    void clearClickHandles();

private:
    QGraphicsSvgItem* m_svgItem;
    QSvgRenderer* m_svgRender;
    QGraphicsRectItem* m_pageRectangle;

};// class QGISVGTemplate

}// namespace TechDrawGui

#endif// DRAWINGGUI_QGRAPHICSITEMSVGTEMPLATE_H
