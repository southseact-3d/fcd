// This custom widget adds the missing ellipsize functionality in QT5

#include "ElideLabel.h"

namespace Gui
{

ElideLabel::ElideLabel(QWidget* parent)
    : QLabel(parent)
{}

void ElideLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(palette().color(QPalette::WindowText));
    painter.setFont(font());

    constexpr int padding = 4;
    QFontMetrics fm(painter.fontMetrics());

    int availableWidth = width() - padding * 2;
    if (availableWidth < 0) {
        return;
    }

    QString elidedText = fm.elidedText(text(), Qt::ElideRight, availableWidth);

    QRect textRect = rect().adjusted(padding, 0, -padding, 0);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
}

QSize ElideLabel::sizeHint() const
{
    QFontMetrics fm(font());
    int width = fm.horizontalAdvance(this->text());
    int height = fm.height();
    return {width, height};
}

QSize ElideLabel::minimumSizeHint() const
{
    QFontMetrics fm(font());
    QString minimumText = QStringLiteral("A...");
    int width = fm.horizontalAdvance(minimumText);
    int height = fm.height();
    return {width, height};
}

}  // namespace Gui

#include "moc_ElideLabel.cpp"  // NOLINT
