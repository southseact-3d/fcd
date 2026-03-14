#ifndef DRAWINGGUI_TEMPLATETEXTFIELD_H
#define DRAWINGGUI_TEMPLATETEXTFIELD_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>

#include "QGIUserTypes.h"

namespace TechDraw {
class DrawTemplate;
}

namespace TechDrawGui
{
    /// QGraphicsItemGroupm-derived class for the text fields in title blocks
    /*!
     * Makes an area on the drawing that's clickable, so appropriate
     * Properties of the template can be modified.
     */
class TechDrawGuiExport TemplateTextField : public QGraphicsItemGroup
{
    public:
        TemplateTextField(QGraphicsItem *parent,
                          TechDraw::DrawTemplate *myTmplte,
                          const std::string &myFieldName);

        ~TemplateTextField() override = default;

        enum {Type = UserType::TemplateTextField};
        int type() const override { return Type;}

        /// Returns the field name that this TemplateTextField represents
        std::string fieldName() const { return fieldNameStr; }

        void setAutofill(const QString& autofillString);
        void setRectangle(QRectF rect);
        void setLine(QPointF from, QPointF to);
        void setLineColor(QColor color);
        void hideLine() { m_line->hide(); }
        void showLine() { m_line->show(); }

    protected:
        /// Need this to properly handle mouse release
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

        /// Trigger the dialog for editing template text
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    private:
        TechDraw::DrawTemplate *tmplte;
        std::string fieldNameStr;
        QString m_autofillString;

        QGraphicsRectItem* m_rect;
        QGraphicsPathItem* m_line;
};
}   // namespace TechDrawGui

#endif // #ifndef DRAWINGGUI_TEMPLATETEXTFIELD_H
