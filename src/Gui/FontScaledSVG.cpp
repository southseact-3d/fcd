// This custom widget scales an svg according to fonts

#include "FontScaledSVG.h"

namespace Gui
{

FontScaledSVG::FontScaledSVG(QWidget* parent)
    : QWidget(parent)
    , m_svgRenderer(new QSvgRenderer(this))
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void FontScaledSVG::setSvg(const QString& svgPath)
{
    if (m_svgRenderer->load(svgPath)) {
        updateScaledSize();
        update();
    }
}

void FontScaledSVG::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (m_svgRenderer->isValid()) {
        QRect targetRect(0, 0, width(), height());
        m_svgRenderer->render(&painter, targetRect);
    }
}

void FontScaledSVG::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    updateScaledSize();
}

void FontScaledSVG::updateScaledSize()
{
    QSize baseSize = m_svgRenderer->defaultSize();

    QFontMetrics metrics(font());
    qreal spacing = metrics.lineSpacing();
    constexpr int baseFactor = 18;
    qreal scalingFactor = spacing / baseFactor;

    QSize targetSize = baseSize * scalingFactor;
    setFixedSize(targetSize);
}

}  // namespace Gui

#include "moc_FontScaledSVG.cpp"  // NOLINT
