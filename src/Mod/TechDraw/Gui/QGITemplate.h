#ifndef DRAWINGGUI_QGRAPHICSITEMTEMPLATE_H
#define DRAWINGGUI_QGRAPHICSITEMTEMPLATE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItemGroup>
#include <QObject>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
QT_END_NAMESPACE

namespace TechDraw {
class DrawTemplate;
}

namespace TechDrawGui
{
class TemplateTextField;
class QGSPage;

class TechDrawGuiExport QGITemplate : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    QGITemplate(QGSPage *);
    ~QGITemplate() override;

    enum {Type = UserType::QGITemplate};
    int type() const override { return Type;}

    void clearContents();

    void setTemplate(TechDraw::DrawTemplate *obj);
    TechDraw::DrawTemplate * getTemplate() { return pageTemplate; }

    inline qreal getY() { return y() * -1; }

    virtual void updateView(bool update = false);
    virtual std::vector<TemplateTextField *> getTextFields() { return textFields; };

    virtual void draw() = 0;

protected:
    TechDraw::DrawTemplate *pageTemplate;

    std::vector<TemplateTextField *> textFields;
};

} // namespace

#endif // DRAWINGGUI_QGRAPHICSITEMTEMPLATE_H
