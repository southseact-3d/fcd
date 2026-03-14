// This custom widget scales an svg according to fonts

#ifndef FONTSCALEDSVG_H
#define FONTSCALEDSVG_H

#include <QWidget>
#include <QSvgRenderer>
#include <QPainter>
#include <QFontMetrics>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport FontScaledSVG: public QWidget
{
    Q_OBJECT

public:
    explicit FontScaledSVG(QWidget* parent = nullptr);
    void setSvg(const QString& svgPath);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QSvgRenderer* m_svgRenderer;

    void updateScaledSize();
};

}  // namespace Gui

#endif  // FONTSCALEDSVG_H
