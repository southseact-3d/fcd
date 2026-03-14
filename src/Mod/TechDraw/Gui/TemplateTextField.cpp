  #include <QGraphicsSceneMouseEvent>
  #include <QInputDialog>
  #include <QLineEdit>
  #include <QTextDocument>

#include <Base/Console.h>
#include <Gui/MainWindow.h>

#include <Mod/TechDraw/App/DrawTemplate.h>
#include <Mod/TechDraw/App/DrawSVGTemplate.h>

#include "DlgTemplateField.h"
#include "TemplateTextField.h"

using namespace TechDrawGui;
using namespace TechDraw;

TemplateTextField::TemplateTextField(QGraphicsItem *parent,
                                     TechDraw::DrawTemplate *myTmplte,
                                     const std::string &myFieldName)
    : QGraphicsItemGroup(parent),
      tmplte(myTmplte),
      fieldNameStr(myFieldName),
      m_rect(new QGraphicsRectItem()),
      m_line(new QGraphicsPathItem())
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
    setFiltersChildEvents(true);

    setToolTip(QObject::tr("Click to update text"));

    addToGroup(m_rect);
    QPen rectPen(Qt::transparent);
    QBrush rectBrush(Qt::NoBrush);
    m_rect->setPen(rectPen);
    m_rect->setBrush(rectBrush);
    m_rect->setAcceptHoverEvents(true);

    m_line->hide();
    addToGroup(m_line);
 }

void TemplateTextField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( tmplte && m_rect->rect().contains(event->pos()) ) {
        event->accept();
    } else {
        QGraphicsItemGroup::mousePressEvent(event);
    }
}

void TemplateTextField::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( tmplte && m_rect->rect().contains(event->pos()) ) {
        event->accept();

        DlgTemplateField ui(Gui::getMainWindow());

        ui.setFieldName(fieldNameStr);
        ui.setFieldContent(tmplte->EditableTexts[fieldNameStr]);

        auto qName = QString::fromStdString(fieldNameStr);
        auto svgTemplate = freecad_cast<DrawSVGTemplate*>(tmplte);
        if (svgTemplate) {
            // preset the autofill with the current value - something might have changed since this field was created
            m_autofillString = svgTemplate->getAutofillByEditableName(qName);
        }
        ui.setAutofillContent(m_autofillString.toStdString());

        if (ui.exec() == QDialog::Accepted) {
            QString qsClean = ui.getFieldContent();
            std::string utf8Content = qsClean.toUtf8().constData();
            if (ui.getAutofillState()) {
                if (svgTemplate) {
                    // unlikely, but something could have changed since we grabbed the autofill value
                    QString fieldName = QString::fromStdString(fieldNameStr);
                    QString autofillValue = svgTemplate->getAutofillByEditableName(fieldName);
                    if (!autofillValue.isEmpty()) {
                        utf8Content = autofillValue.toUtf8().constData();
                    }
                }
            }
            tmplte->EditableTexts.setValue(fieldNameStr, utf8Content);
        }

    } else {
        QGraphicsItemGroup::mouseReleaseEvent(event);
    }
}

//void setAutofill(std::string autofillString);
void TemplateTextField::setAutofill(const QString& autofillString)
{
    m_autofillString = autofillString;
}


void TemplateTextField::setRectangle(QRectF rect)
{
    m_rect->setRect(rect);
}

void TemplateTextField::setLine(QPointF from, QPointF to)
{
    QPainterPath path(from);
    path.lineTo(to);
    m_line->setPath(path);
}

void TemplateTextField::setLineColor(QColor color)
{
    QPen pen(color);
    constexpr int LineWidth{5};
    pen.setWidth(LineWidth);
    m_line->setPen(pen);
}

void TemplateTextField::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    showLine();
    QGraphicsItemGroup::hoverEnterEvent(event);
}

void TemplateTextField::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideLine();
    QGraphicsItemGroup::hoverLeaveEvent(event);
}


